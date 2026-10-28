#!/usr/bin/env python3
"""Summarize the archived Day 7 CSV; never run a matrix benchmark."""

import argparse
import csv
import hashlib
import json
import math
from pathlib import Path
import platform
import statistics

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv"
SIZES = (64, 128, 256)
ALGORITHMS = ("ijk", "ikj", "blocked_16", "blocked_32", "blocked_64")


def load_samples(path):
    """Fail closed: no dropping failed, duplicated, or malformed observations."""
    groups = {(n, algorithm): [] for n in SIZES for algorithm in ALGORITHMS}
    seen = set()
    checksums = {}
    with path.open(encoding="utf-8-sig", newline="") as stream:
        rows = list(csv.DictReader(stream))
    if len(rows) != 75:
        raise ValueError(f"Expected 75 records, got {len(rows)}")
    for row in rows:
        n, algorithm, sample = int(row["n"]), row["algorithm"], int(row["sample"])
        key = (n, algorithm, sample)
        if (n, algorithm) not in groups or sample not in range(1, 6):
            raise ValueError(f"Unexpected configuration/sample: {key}")
        if key in seen:
            raise ValueError(f"Duplicate sample: {key}")
        seen.add(key)
        if row["status"] != "PASS" or row["elementwise_match"] != "YES":
            raise ValueError(f"Failed correctness record: {key}")
        c00, expected = int(row["C00"]), int(row["expected_C00"])
        formula_c00 = sum((k + 1) * (2 * k + 1) for k in range(n))
        if c00 != expected or expected != formula_c00:
            raise ValueError(f"Unexpected C00: {key}")
        checksum = int(row["checksum"])
        if checksum != checksums.setdefault(n, checksum):
            raise ValueError(f"Inconsistent checksum for n={n}")
        elapsed = float(row["ms_per_multiply"])
        total, pilot = float(row["total_ms"]), float(row["pilot_ms"])
        repeats = int(row["repeats"])
        if repeats <= 0 or any(not math.isfinite(v) or v <= 0 for v in (elapsed, total, pilot)):
            raise ValueError(f"Nonpositive or nonfinite timing: {key}")
        # The source prints six decimal places. Allow only rounding-sized error.
        if abs(total / repeats - elapsed) > 0.000001:
            raise ValueError(f"total_ms / repeats disagrees with timing: {key}")
        groups[n, algorithm].append((sample, elapsed))
    for key, samples in groups.items():
        if len(samples) != 5:
            raise ValueError(f"Expected five samples: {key}")
        samples.sort()
    return groups


def summarize(groups):
    rows = []
    for n in SIZES:
        baseline = statistics.median(value for _, value in groups[n, "ikj"])
        for algorithm in ALGORITHMS:
            times = [value for _, value in groups[n, algorithm]]
            median = statistics.median(times)
            rows.append({
                "n": n, "algorithm": algorithm, "samples": len(times),
                "min_ms": min(times), "median_ms": median, "max_ms": max(times),
                "ikj_median_ms": baseline,
                "speedup_vs_ikj": baseline / median,
                "time_change_vs_ikj_percent": (median / baseline - 1) * 100,
            })
    return rows


def draw(groups, rows, output):
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt

    by_key = {(row["n"], row["algorithm"]): row for row in rows}
    colors = ("#595959", "#0072B2", "#8C510A", "#6A3D9A", "#006B54")
    with plt.rc_context({"font.family": "DejaVu Sans", "font.size": 10,
                         "svg.fonttype": "none", "svg.hashsalt": "day07-v1"}):
        fig, axes = plt.subplots(1, 3, figsize=(12, 4.8), sharex=True, sharey=True)
        fig.subplots_adjust(left=0.12, right=0.97, bottom=0.27, top=0.75, wspace=0.14)
        for ax, n in zip(axes, SIZES):
            baseline = by_key[n, "ikj"]["median_ms"]
            for y, (algorithm, color) in enumerate(zip(ALGORITHMS, colors)):
                row = by_key[n, algorithm]
                ratios = [baseline / value for _, value in groups[n, algorithm]]
                # All five observations, slight deterministic vertical offsets.
                ax.scatter(ratios, [y + (i - 2) * 0.055 for i in range(5)],
                           s=17, color=color, alpha=0.75, zorder=3)
                ax.hlines(y, min(ratios), max(ratios), color=color, linewidth=1.4)
                ax.scatter([row["speedup_vs_ikj"]], [y], marker="|", s=180,
                           color="black", linewidths=2, zorder=4)
                ax.annotate(f'{row["speedup_vs_ikj"]:.3f}x',
                            (max(ratios) + 0.025, y), va="center", fontsize=9)
            ax.axvline(1, color="#303030", linestyle="--", linewidth=1)
            ax.set_title(f"n = {n}\nikj median = {baseline:.6f} ms", fontsize=11)
            ax.set_yticks(range(5), ALGORITHMS)
            ax.set_xlim(0, 1.28)
            ax.set_ylim(4.5, -0.5)
            ax.set_xticks([0, 0.25, 0.5, 0.75, 1])
            ax.grid(axis="x", color="#dddddd", linewidth=0.6)
            ax.spines[["top", "right"]].set_visible(False)
        fig.suptitle("Does blocking beat ikj? Not at n = 128 or 256 in this run.",
                     fontsize=14, x=0.52, y=0.97)
        fig.text(0.52, 0.86,
                 "Archived CPU measurements, 2026-10-27 | 75 records | 5 timed samples per configuration",
                 ha="center", fontsize=10)
        fig.text(0.52, 0.17,
                 "Speed relative to ikj = median(ikj time) / algorithm time; larger is faster",
                 ha="center", fontsize=11)
        fig.text(0.12, 0.08,
                 "Dots: all samples; line: min-max; black tick + label: ratio of medians. Ranges are not confidence intervals.\n"
                 "Each panel uses its own ikj median. At n=64 the ranges overlap. Clearing C is included in timing.\n"
                 "Archived executable/source match and exact compiler flags remain unverified; this is not NPU performance.",
                 va="center", fontsize=9)
        fig.savefig(output / "day07-speedup.svg", facecolor="white",
                    metadata={"Date": None, "Creator": "scripts/summarize_day07.py"})
        plt.close(fig)
    # Matplotlib emits trailing spaces in SVG path attributes. Newlines retain
    # the required token separators; normalize only line-end whitespace.
    svg = output / "day07-speedup.svg"
    svg.write_bytes(("\n".join(line.rstrip() for line in
                              svg.read_text(encoding="utf-8").splitlines()) + "\n").encode("utf-8"))
    return matplotlib.__version__


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--csv", type=Path, default=SOURCE)
    parser.add_argument("--output-dir", type=Path, default=ROOT / "experiments/figures")
    args = parser.parse_args()
    try:
        groups = load_samples(args.csv)
        rows = summarize(groups)
    except (ValueError, KeyError, OSError) as error:
        parser.error(str(error))
    args.output_dir.mkdir(parents=True, exist_ok=True)
    with (args.output_dir / "day07-summary.csv").open("w", encoding="utf-8", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=rows[0].keys(), lineterminator="\n")
        writer.writeheader()
        for row in rows:
            writer.writerow({key: f"{value:.9f}" if isinstance(value, float) else value
                             for key, value in row.items()})
    mpl_version = draw(groups, rows, args.output_dir)
    try:
        source_label = args.csv.resolve().relative_to(ROOT).as_posix()
    except ValueError:
        source_label = args.csv.name
    provenance = {
        "source": source_label,
        "source_sha256": hashlib.sha256(args.csv.read_bytes()).hexdigest(),
        "generator": "scripts/summarize_day07.py",
        "generator_sha256": hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
        "python": platform.python_version(), "matplotlib": mpl_version,
        "rows": 75, "groups": 15, "samples_per_group": 5, "excluded_rows": 0,
        "timing_column": "ms_per_multiply",
        "aggregation": "median, min, max; all five timed samples retained",
        "speedup_definition": "median(ikj time) / median(algorithm time), within each n",
        "figure_points": "median(ikj time) / individual algorithm time, within each n",
        "range_definition": "observed min-max of the normalized samples; not a confidence interval",
        "measurements": "historical user CSV dated 2026-10-27; no new benchmark executed",
        "limitations": [
            "Original executed source is not available verbatim; reconstructed src is GPT-integrated.",
            "Exact original toolset and compiler flags are not recorded.",
            "Current integrated src uses fixed stride 256 and int arrays without explicit parallelism; this does not prove original binary configuration.",
            "No hardware cache counter or real NPU measurement is present.",
        ],
    }
    (args.output_dir / "day07-provenance.json").write_text(
        json.dumps(provenance, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    print("PASS: 75 records, 15 groups, five samples each; no rows excluded.")
    print(f"Wrote summary CSV, SVG and provenance to {args.output_dir}")


if __name__ == "__main__":
    main()
