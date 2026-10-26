#!/usr/bin/env python3
"""编译 benchmarks/kernels 里的全部实现，测量并画到同一张图。

脚本不提供新算法。当前目录里只有与 src/day02_matrix.cpp 相同的 i-j-k 循环。
以后把新的 .cpp 放进 benchmarks/kernels/，再跑这一条命令即可。
"""

from __future__ import annotations

import argparse
import csv
import datetime as dt
import math
import os
import re
import statistics
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
BUILD = ROOT / "build"
KERNEL_DIR = ROOT / "benchmarks" / "kernels"
BENCH_SRC = ROOT / "benchmarks" / "matmul_bench.cpp"
DAY02_SRC = ROOT / "src" / "day02_matrix.cpp"
CXX_FLAGS = ["-O2", "-std=c++17", "-Wall", "-Wextra", "-Werror"]
# 同一循环体若落在不同对齐上，这台机器上的耗时可以差出将近一倍。
# 只加在计时编译上，不改变算法。2026-10-26 已发表的数据没有这个选项。
BENCH_FLAGS = CXX_FLAGS + ["-falign-loops=32"]
ID_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def run(cmd: list[str], cwd: Path | None = None, capture: bool = False) -> subprocess.CompletedProcess[str]:
    printable = " ".join(cmd)
    print(f"+ {printable}", flush=True)
    proc = subprocess.run(
        cmd,
        cwd=cwd,
        check=False,
        text=True,
        capture_output=capture,
    )
    if proc.returncode != 0:
        if capture:
            sys.stderr.write(proc.stdout or "")
            sys.stderr.write(proc.stderr or "")
        raise SystemExit(proc.returncode)
    return proc


def capture_text(cmd: list[str]) -> str:
    proc = subprocess.run(cmd, check=False, text=True, capture_output=True)
    if proc.returncode != 0:
        return (proc.stderr or proc.stdout or "").strip()
    return proc.stdout.strip()


@dataclass(frozen=True)
class Kernel:
    path: Path
    bench_id: str
    label: str
    label_en: str


def cpp_string(text: str) -> str:
    escaped = text.replace("\\", "\\\\").replace('"', '\\"')
    return f'"{escaped}"'


def parse_kernel_file(path: Path) -> Kernel:
    fields: dict[str, str] = {}
    text = path.read_text(encoding="utf-8")
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped.startswith("//"):
            continue
        body = stripped[2:].strip()
        if ":" not in body:
            continue
        key, value = body.split(":", 1)
        key = key.strip()
        if key in {"BENCH_ID", "BENCH_LABEL", "BENCH_LABEL_EN"} and key not in fields:
            fields[key] = value.strip()
    bench_id = fields.get("BENCH_ID", "")
    label = fields.get("BENCH_LABEL", "")
    if not ID_RE.fullmatch(bench_id):
        raise SystemExit(f"{path} 缺少合法的 BENCH_ID（字母或下划线开头）")
    if not label:
        raise SystemExit(f"{path} 缺少 BENCH_LABEL")
    if re.search(rf"\bmatmul_{re.escape(bench_id)}\s*\(", text) is None:
        raise SystemExit(f"{path} 需要定义函数 matmul_{bench_id}")
    return Kernel(path, bench_id, label, fields.get("BENCH_LABEL_EN") or bench_id)


def discover_kernels(directory: Path | None = None) -> list[Kernel]:
    folder = directory or KERNEL_DIR
    files = sorted(path for path in folder.glob("*.cpp") if path.is_file())
    if not files:
        raise SystemExit(f"{folder} 里没有 .cpp 实现")
    kernels = [parse_kernel_file(path) for path in files]
    seen: dict[str, Path] = {}
    for kernel in kernels:
        if kernel.bench_id in seen:
            raise SystemExit(f"BENCH_ID 重复: {kernel.bench_id}（{seen[kernel.bench_id]} 与 {kernel.path}）")
        seen[kernel.bench_id] = kernel.path
    return kernels


def write_registry(kernels: list[Kernel], path: Path) -> None:
    lines = [
        "#include \"kernel_api.h\"",
        "",
    ]
    for kernel in kernels:
        lines.append(f"void matmul_{kernel.bench_id}(const int*, const int*, int*, int);")
    lines.extend(["", "static const BenchImpl kImpls[] = {"])
    for kernel in kernels:
        lines.append(
            f"    {{{cpp_string(kernel.bench_id)}, {cpp_string(kernel.label)}, matmul_{kernel.bench_id}}},"
        )
    lines.extend(
        [
            "};",
            "",
            "int bench_impl_count() {",
            "    return static_cast<int>(sizeof(kImpls) / sizeof(kImpls[0]));",
            "}",
            "",
            "const BenchImpl* bench_impl_at(int index) {",
            "    return &kImpls[index];",
            "}",
            "",
        ]
    )
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text("\n".join(lines), encoding="utf-8")


def display_cmd(cmd: list[str]) -> str:
    shown = []
    for part in cmd:
        candidate = Path(part)
        try:
            shown.append(str(candidate.resolve().relative_to(ROOT)))
        except ValueError:
            shown.append(part)
    return " ".join(shown)


def compile_sources(kernels: list[Kernel]) -> tuple[Path, str]:
    BUILD.mkdir(parents=True, exist_ok=True)
    day02_bin = BUILD / "day02_matrix"
    run(["g++", *CXX_FLAGS, "-o", str(day02_bin), str(DAY02_SRC)])
    day02 = run([str(day02_bin)], capture=True)
    expected = "11 10 \n19 30 \n"
    if day02.stdout != expected:
        sys.stderr.write("src/day02_matrix.cpp 输出与记录不一致\n")
        sys.stderr.write(repr(day02.stdout) + "\n")
        raise SystemExit(1)
    print("src/day02_matrix.cpp 原始输入输出与记录一致", flush=True)

    registry = BUILD / "kernel_registry.cpp"
    write_registry(kernels, registry)
    bench_bin = BUILD / "matmul_bench"
    cmd = [
        "g++",
        *BENCH_FLAGS,
        "-I",
        str(ROOT / "benchmarks"),
        "-o",
        str(bench_bin),
        str(BENCH_SRC),
        str(registry),
        *[str(kernel.path) for kernel in kernels],
    ]
    run(cmd)
    return bench_bin, display_cmd(cmd)


def gflops_of(n: int, elapsed_ns: int, reps: int) -> float:
    time_s = elapsed_ns / reps / 1e9
    flops = 2.0 * n * n * n
    return flops / time_s / 1e9


def load_raw(path: Path) -> list[dict[str, str]]:
    with path.open(newline="", encoding="utf-8") as handle:
        rows = list(csv.DictReader(handle))
    if not rows:
        raise SystemExit(f"{path} 没有数据行")
    for row in rows:
        n = int(row["n"])
        elapsed_ns = int(row["elapsed_ns"])
        reps = int(row["reps"])
        reported = float(row["gflops"])
        recomputed = gflops_of(n, elapsed_ns, reps)
        scale = max(abs(recomputed), 1e-12)
        if abs(reported - recomputed) / scale > 1e-9:
            raise SystemExit(
                f"GFLOPS 复核失败 n={n} sample={row['sample']}: "
                f"csv={reported} recomputed={recomputed}"
            )
    return rows


def summarize(rows: list[dict[str, str]]) -> list[dict[str, object]]:
    grouped: dict[tuple[str, int], list[dict[str, str]]] = {}
    order: list[tuple[str, int]] = []
    for row in rows:
        key = (row["impl"], int(row["n"]))
        if key not in grouped:
            grouped[key] = []
            order.append(key)
        grouped[key].append(row)

    summary: list[dict[str, object]] = []
    previous_median: dict[str, float] = {}
    for impl, n in order:
        group = grouped[(impl, n)]
        checksums = {int(row["checksum"]) for row in group}
        if len(checksums) != 1:
            raise SystemExit(f"{impl} n={n} 的校验和不一致: {checksums}")
        reps_values = {int(row["reps"]) for row in group}
        if len(reps_values) != 1:
            raise SystemExit(f"{impl} n={n} 的重复次数不一致: {reps_values}")
        elapsed = [int(row["elapsed_ns"]) for row in group]
        reps = next(iter(reps_values))
        per_call_ns = [value / reps for value in elapsed]
        median_ns = statistics.median(per_call_ns)
        mean_ns = statistics.fmean(per_call_ns)
        min_ns = min(per_call_ns)
        max_ns = max(per_call_ns)

        def to_gflops(sample_ns: float) -> float:
            time_s = sample_ns / 1e9
            return (2.0 * n * n * n) / time_s / 1e9

        median_time_s = median_ns / 1e9
        ratio = ""
        if impl in previous_median and previous_median[impl] > 0:
            ratio = f"{median_time_s / previous_median[impl]:.6f}"
        previous_median[impl] = median_time_s
        summary.append(
            {
                "impl": impl,
                "n": n,
                "runs": len(group),
                "reps": reps,
                "median_time_s": median_time_s,
                "mean_time_s": mean_ns / 1e9,
                "min_time_s": min_ns / 1e9,
                "max_time_s": max_ns / 1e9,
                "gflops_median": to_gflops(median_ns),
                "gflops_mean": to_gflops(mean_ns),
                "gflops_min": to_gflops(max_ns),
                "gflops_max": to_gflops(min_ns),
                "time_ratio_vs_prev": ratio,
                "checksum": next(iter(checksums)),
            }
        )
    return summary


def write_summary(path: Path, summary: list[dict[str, object]]) -> None:
    fields = [
        "impl",
        "n",
        "runs",
        "reps",
        "median_time_s",
        "mean_time_s",
        "min_time_s",
        "max_time_s",
        "gflops_median",
        "gflops_mean",
        "gflops_min",
        "gflops_max",
        "time_ratio_vs_prev",
        "checksum",
    ]
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        for row in summary:
            encoded = dict(row)
            for key, value in encoded.items():
                if isinstance(value, float):
                    encoded[key] = f"{value:.10g}"
            writer.writerow(encoded)


def configure_font() -> tuple[str, str]:
    os.environ.setdefault("MPLBACKEND", "Agg")
    from matplotlib import font_manager
    import matplotlib.pyplot as plt

    candidates = [
        "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
        "/usr/share/fonts/opentype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
        "/usr/share/fonts/truetype/droid/DroidSansFallbackFull.ttf",
    ]
    for path in candidates:
        if os.path.isfile(path):
            font_manager.fontManager.addfont(path)
            name = font_manager.FontProperties(fname=path).get_name()
            plt.rcParams["font.family"] = name
            plt.rcParams["axes.unicode_minus"] = False
            return "zh", f"{name} ({path})"
    plt.rcParams["axes.unicode_minus"] = False
    return "en", "default"


def ensure_matplotlib() -> None:
    try:
        import matplotlib  # noqa: F401
    except ModuleNotFoundError:
        print("未找到 matplotlib，尝试安装", flush=True)
        run([sys.executable, "-m", "pip", "install", "--user", "matplotlib"])


def plot_summary(
    summary: list[dict[str, object]],
    path: Path,
    labels_zh: dict[str, str] | None = None,
    labels_en: dict[str, str] | None = None,
) -> str:
    ensure_matplotlib()
    language, font_desc = configure_font()
    import matplotlib.pyplot as plt

    impls = []
    for row in summary:
        if row["impl"] not in impls:
            impls.append(row["impl"])
    labels_zh = labels_zh or {}
    labels_en = labels_en or {}
    labels = labels_zh if language == "zh" else labels_en

    if language == "zh":
        xlabel = "矩阵规模 n"
        ylabel_time = "单次耗时（毫秒）"
        ylabel_gflops = "GFLOPS"
        note = "GFLOPS = 2*n^3 / 单次耗时(秒) / 1e9；整数乘加，误差线为样本最小/最大。仅供相对比较。"
        if len(impls) == 1:
            title = f"{labels.get(str(impls[0]), impls[0])}在不同矩阵规模下的性能"
        else:
            title = "各实现在不同矩阵规模下的性能"
    else:
        xlabel = "Matrix size n"
        ylabel_time = "Time per call (ms)"
        ylabel_gflops = "GFLOPS"
        note = "GFLOPS = 2*n^3 / time(s) / 1e9 for integer multiply-adds. Error bars are min/max. Relative only."
        if len(impls) == 1:
            title = f"{labels.get(str(impls[0]), impls[0])} across matrix sizes"
        else:
            title = "Implementations across matrix sizes"
    time_scale = 1e3

    def log_tick(value, _pos):
        if value <= 0:
            return ""
        exponent = round(math.log10(value))
        if abs(value - 10**exponent) / value < 1e-6:
            return f"1e{exponent}" if exponent != 0 else "1"
        return f"{value:g}"

    fig, axes = plt.subplots(1, 2, figsize=(10.2, 4.4))
    colors = ["#1f4e79", "#c45911", "#548235", "#7030a0"]
    for index, impl in enumerate(impls):
        rows = [row for row in summary if row["impl"] == impl]
        ns = [int(row["n"]) for row in rows]
        med_t = [float(row["median_time_s"]) * time_scale for row in rows]
        min_t = [float(row["min_time_s"]) * time_scale for row in rows]
        max_t = [float(row["max_time_s"]) * time_scale for row in rows]
        med_g = [float(row["gflops_median"]) for row in rows]
        min_g = [float(row["gflops_min"]) for row in rows]
        max_g = [float(row["gflops_max"]) for row in rows]
        color = colors[index % len(colors)]
        label = labels.get(str(impl), str(impl))
        axes[0].errorbar(
            ns,
            med_t,
            yerr=[[m - lo for m, lo in zip(med_t, min_t)], [hi - m for m, hi in zip(med_t, max_t)]],
            marker="o",
            color=color,
            capsize=3,
            label=label,
        )
        axes[1].errorbar(
            ns,
            med_g,
            yerr=[[m - lo for m, lo in zip(med_g, min_g)], [hi - m for m, hi in zip(med_g, max_g)]],
            marker="o",
            color=color,
            capsize=3,
            label=label,
        )
        if len(impls) == 1:
            for n, gflops in zip(ns, med_g):
                axes[1].annotate(
                    f"{gflops:.2f}",
                    (n, gflops),
                    textcoords="offset points",
                    xytext=(0, 8),
                    ha="center",
                    fontsize=8,
                    color=color,
                )

    sizes = sorted({int(row["n"]) for row in summary})
    for axis in axes:
        axis.set_xscale("log", base=2)
        axis.set_xticks(sizes)
        axis.set_xticklabels([str(n) for n in sizes])
        axis.set_xlabel(xlabel)
        axis.grid(True, which="both", linestyle="--", alpha=0.35)
        axis.legend()
    axes[0].set_yscale("log")
    axes[0].yaxis.set_major_formatter(plt.FuncFormatter(log_tick))
    axes[0].set_ylabel(ylabel_time)
    gmax = max(float(row["gflops_max"]) for row in summary)
    axes[1].set_ylim(0, gmax * 1.28)
    axes[1].set_ylabel(ylabel_gflops)
    fig.suptitle(title)
    fig.tight_layout(rect=(0, 0.07, 1, 0.92))
    fig.text(0.5, 0.012, note, ha="center", va="bottom", fontsize=8, color="#444444")
    path.parent.mkdir(parents=True, exist_ok=True)
    fig.savefig(path, dpi=140)
    plt.close(fig)
    print(f"已写入 {path}（标签语言: {language}, 字体: {font_desc}）", flush=True)
    return font_desc


def write_environment(
    path: Path,
    args: argparse.Namespace,
    compile_cmd: str,
    font_desc: str,
    raw_name: str,
    summary_name: str,
    figure_name: str,
    kernels: list[Kernel],
) -> None:
    status = capture_text(["git", "status", "--porcelain"])
    lines = [
        f"measured_at_utc: {dt.datetime.now(dt.timezone.utc).strftime('%Y-%m-%dT%H:%M:%SZ')}",
        f"os: {capture_text(['bash', '-lc', '. /etc/os-release && echo \"$PRETTY_NAME\"'])}",
        f"uname: {capture_text(['uname', '-a'])}",
        f"compiler: {capture_text(['g++', '--version']).splitlines()[0]}",
        f"compile_command: {compile_cmd}",
        "lto: off",
        "timing: std::chrono::steady_clock；只计各实现函数的连续调用。输入构造、校准、正确性检查不在正式样本内。现有 ijk 在函数内把 C 清零再乘加",
        "repetition: 单次调用短于 min_ms 时，在同一样本内连续重复，单次耗时 = 样本总耗时 / reps；校准跑不写入 CSV",
        "gflops: 2 * n^3 / time_per_call_seconds / 1e9；一次乘法加一次加法。内核是 32 位整数",
        f"algorithms: {', '.join(kernel.bench_id + ' (' + kernel.label + ')' for kernel in kernels)}",
        f"sizes: {args.sizes}",
        f"runs: {args.runs}",
        f"min_ms: {args.min_ms}",
        f"git_head: {capture_text(['git', 'rev-parse', 'HEAD'])}",
        f"git_status_porcelain: {status if status else 'clean'}",
        f"python: {sys.version.split()[0]}",
        f"plot_font: {font_desc if font_desc else 'skipped'}",
        f"raw_csv: {raw_name}",
        f"summary_csv: {summary_name}",
        f"figure: {figure_name}",
        "",
        "---- lscpu ----",
        capture_text(["lscpu"]),
        "",
        "---- meminfo (MemTotal) ----",
        capture_text(["bash", "-lc", "awk '/MemTotal/ {print}' /proc/meminfo"]),
    ]
    path.write_text("\n".join(lines) + "\n", encoding="utf-8")


def write_result_readme(
    path: Path,
    summary: list[dict[str, object]],
    font_desc: str,
    kernels: list[Kernel],
) -> None:
    env = (path.parent / "environment.txt").read_text(encoding="utf-8")
    def field(prefix: str) -> str:
        for line in env.splitlines():
            if line.startswith(prefix):
                return line.split(": ", 1)[1]
        return ""

    names = "、".join(f"{kernel.bench_id}（{kernel.label}）" for kernel in kernels)
    table = [
        "| 实现 | 规模 n | 样本内重复 | 正式样本 | 单次耗时中位数 (ms) | 单次耗时最小–最大 (ms) | GFLOPS（中位数耗时） | 相对上一档耗时倍数 |",
        "| --- | ---: | ---: | ---: | ---: | --- | ---: | ---: |",
    ]
    for row in summary:
        ratio = row["time_ratio_vs_prev"] or "—"
        table.append(
            "| {impl} | {n} | {reps} | {runs} | {med:.6f} | {lo:.6f}–{hi:.6f} | {gflops:.6f} | {ratio} |".format(
                impl=row["impl"],
                n=row["n"],
                reps=row["reps"],
                runs=row["runs"],
                med=float(row["median_time_s"]) * 1e3,
                lo=float(row["min_time_s"]) * 1e3,
                hi=float(row["max_time_s"]) * 1e3,
                gflops=float(row["gflops_median"]),
                ratio=ratio,
            )
        )
    text = f"""# 矩阵乘法规模扫描

测了什么：本次命令发现的实现是 {names}。脚本会把 `benchmarks/kernels/*.cpp` 里的每个实现画进同一张图。Day 1 只算一个元素，不参与对比。

条件摘要：

- 时间：{field('measured_at_utc')}
- 系统：{field('os')}
- CPU：见 `environment.txt` 里的 lscpu（云端 VM）
- 编译器：{field('compiler')}
- 编译命令：`{field('compile_command')}`
- 图中字体：{font_desc}

计时对象是各实现函数的连续调用。输入构造、校准和正确性检查不在正式样本里。单次调用短于设定的最短样本时长时，同一样本内连续重复，单次耗时 = 样本总耗时 / 重复次数。GFLOPS = 2 * n^3 / 单次耗时秒 / 1e9，把每次乘加计为 1 次乘法和 1 次加法。内核是 32 位整数；数字只适合相对比较，不是浮点峰值。

原始数据：`raw.csv`。汇总：`summary.csv`。图：`perf.png`。完整环境：`environment.txt`。

{chr(10).join(table)}
"""
    path.write_text(text, encoding="utf-8")


def parse_args() -> argparse.Namespace:
    today = dt.datetime.now().strftime("%Y-%m-%d")
    parser = argparse.ArgumentParser(description="测量 benchmarks/kernels 中的矩阵乘法实现")
    parser.add_argument("--sizes", default="64,128,256,512,1024", help="逗号分隔的矩阵规模")
    parser.add_argument("--runs", type=int, default=5, help="每个规模的正式样本数")
    parser.add_argument("--min-ms", type=float, default=100.0, help="每个正式样本尽量达到的最短毫秒数")
    parser.add_argument(
        "--out",
        type=Path,
        default=ROOT / "results" / f"{today}_matmul",
        help="CSV 和图的输出目录",
    )
    parser.add_argument("--skip-plot", action="store_true", help="只编译和测量，不画图")
    parser.add_argument("--list", action="store_true", help="只列出发现的实现，不编译")
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    kernels = discover_kernels()
    if args.list:
        for kernel in kernels:
            print(f"{kernel.bench_id}\t{kernel.label}\t{kernel.path.relative_to(ROOT)}")
        return
    if args.runs <= 0 or args.min_ms <= 0:
        raise SystemExit("--runs 和 --min-ms 必须为正")
    out = args.out if args.out.is_absolute() else ROOT / args.out
    out.mkdir(parents=True, exist_ok=True)
    bench_bin, compile_cmd = compile_sources(kernels)
    run([str(bench_bin), "--check"])
    raw_path = out / "raw.csv"
    run(
        [
            str(bench_bin),
            "--sizes",
            args.sizes,
            "--runs",
            str(args.runs),
            "--min-ms",
            str(args.min_ms),
            "--csv",
            str(raw_path),
        ]
    )
    rows = load_raw(raw_path)
    summary = summarize(rows)
    summary_path = out / "summary.csv"
    write_summary(summary_path, summary)
    labels_zh = {kernel.bench_id: kernel.label for kernel in kernels}
    labels_en = {kernel.bench_id: kernel.label_en for kernel in kernels}
    font_desc = ""
    figure_name = ""
    if not args.skip_plot:
        font_desc = plot_summary(summary, out / "perf.png", labels_zh, labels_en)
        figure_name = "perf.png"
    write_environment(
        out / "environment.txt",
        args,
        compile_cmd,
        font_desc,
        "raw.csv",
        "summary.csv",
        figure_name or "(skipped)",
        kernels,
    )
    if not args.skip_plot:
        write_result_readme(out / "README.md", summary, font_desc, kernels)
    print(f"结果目录: {out}", flush=True)


if __name__ == "__main__":
    main()
