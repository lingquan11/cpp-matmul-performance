#!/usr/bin/env python3
"""Plot archived compiler comparison batches, grouped by process run."""
import csv
import math
import statistics
from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

ROOT = Path(__file__).resolve().parents[1]
DATA = ROOT / 'experiments/results/2026-11-01-compiler-blocking-windows'
OUT = ROOT / 'experiments/figures/compiler-blocking-2026-11-01.svg'
RUNS = [('01-o2', '#0072B2'), ('02-od', '#A94E00'), ('03-od', '#A94E00'), ('04-o2', '#0072B2')]
ALGORITHMS = ('ijk', 'ikj', 'blocked_16', 'blocked_32', 'blocked_64')
SIZES = (64, 128, 256)

def load_data():
    groups = {}
    for run, _ in RUNS:
        with (DATA / (run + '.csv')).open(encoding='utf-8-sig', newline='') as f:
            rows = list(csv.DictReader(f))
        if len(rows) != 75:
            raise ValueError(f'{run}: expected 75 rows')
        seen = set()
        for row in rows:
            n, alg, sample = int(row['n']), row['algorithm'], int(row['sample'])
            key = (n, alg, sample)
            if n not in SIZES or alg not in ALGORITHMS or sample not in range(1, 6) or key in seen:
                raise ValueError(f'{run}: invalid or duplicate sample {key}')
            seen.add(key)
            if row['status'] != 'PASS' or row['elementwise_match'] != 'YES':
                raise ValueError(f'{run}: failed sample {key}')
            value = float(row['ms_per_multiply'])
            if not math.isfinite(value) or value <= 0:
                raise ValueError(f'{run}: invalid time {key}')
            groups.setdefault((run, n, alg), []).append((sample, value))
    if any(len(groups.get((run, n, alg), [])) != 5 for run, _ in RUNS for n in SIZES for alg in ALGORITHMS):
        raise ValueError('Incomplete group')
    return groups

def make_plot(groups, preview_path=None):
    with plt.rc_context({'font.family': 'DejaVu Sans', 'font.size': 10, 'svg.fonttype': 'none', 'svg.hashsalt': 'compiler-blocking-20261101'}):
        fig, axes = plt.subplots(1, 3, figsize=(13.2, 7.3), sharey=True, sharex=True)
        fig.subplots_adjust(left=.13, right=.98, top=.82, bottom=.23, wspace=.12)
        for ax, n in zip(axes, SIZES):
            for y, alg in enumerate(ALGORITHMS):
                for off, (run, color) in zip([-.27, -.09, .09, .27], RUNS):
                    vals = [v for _, v in sorted(groups[run, n, alg])]
                    ax.plot(vals, [y+off]*5, linestyle='none', marker='o', ms=3.5, color=color, alpha=.70)
                    ax.plot(statistics.median(vals), y+off, marker='|', markersize=12, color='black', markeredgewidth=1.7)
            ax.set_xscale('log')
            ax.set_xlim(.015, 70)
            ax.set_xticks([.02, .1, 1, 10, 50], labels=['0.02', '0.1', '1', '10', '50'])
            ax.set_title(f'n = {n}', fontsize=12, fontweight='bold')
            ax.set_yticks(range(len(ALGORITHMS)), ALGORITHMS)
            ax.set_ylim(4.55, -.55)
            ax.grid(axis='x', which='major', color='#dddddd', linewidth=.7)
            ax.spines[['top', 'right']].set_visible(False)
        fig.suptitle('Compiler settings and matrix blocking: four process runs', fontsize=16, x=.53, y=.97)
        fig.text(.53, .895, '2026-11-01  |  MSVC x64  |  source: Day 7 integrated version', ha='center', fontsize=10)
        fig.text(.53, .855, 'Each algorithm, top to bottom: /O2 run 1, /Od run 2, /Od run 3, /O2 run 4', ha='center', fontsize=10)
        fig.supxlabel('Time per multiply (ms, logarithmic scale)  —  lower is faster', x=.54, y=.16, fontsize=11)
        fig.text(.13, .045, 'Dots = five batch averages within each run; black tick = median of those five. No samples excluded.\n'
                 'Only two process runs per setting; frequency, background load and affinity were not controlled.\n'
                 'Timing includes clearing C; this does not measure cache misses or reproduce the historical executable.', fontsize=9, va='center')
        OUT.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(OUT, facecolor='white', metadata={'Date': None, 'Creator': 'scripts/plot-compiler-blocking.py'})
        if preview_path:
            fig.savefig(preview_path, dpi=140, facecolor='white')
        plt.close(fig)

if __name__ == '__main__':
    import sys
    make_plot(load_data(), Path(sys.argv[1]) if len(sys.argv) > 1 else None)
    print('PASS: 300 records, 60 groups of five; no samples excluded')
    print('Matplotlib', matplotlib.__version__)
