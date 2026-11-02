# AI 维护：读取隔离目录 CSV，生成两张图；fixture 非实测。
# python scripts/plot-scale-sweep.py INPUT_DIR OUTPUT_DIR（需要 matplotlib）
import csv
import statistics
import sys
from collections import defaultdict
from pathlib import Path
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

def main():
    source, output = map(Path, sys.argv[1:3])
    groups = defaultdict(list)
    rates = defaultdict(list)
    for path in source.rglob("*.csv"):
        with path.open(newline="", encoding="utf-8-sig") as f:
            for row in csv.DictReader(f):
                if row.get("skipped_timeout") == "1" or not row.get("ms_per_multiply"): continue
                key = (int(row["n"]), row["algorithm"])
                groups[key].append(float(row["ms_per_multiply"]))
                if row.get("gmac_per_s"): rates[key].append(float(row["gmac_per_s"]))
    if not groups: raise SystemExit("No usable data")
    output.mkdir(parents=True, exist_ok=True)
    fig, ax = plt.subplots()
    for algo in sorted({a for n,a in groups if a.startswith("blocked_")}):
        ns = sorted(n for n,a in groups if a == algo and (n,"ikj") in groups)
        ax.plot(ns, [statistics.median(groups[n,algo])/statistics.median(groups[n,"ikj"]) for n in ns], marker="o", label=algo)
    ax.axhline(1, color="gray"); ax.set(xlabel="n", ylabel="median blocked / ikj"); ax.legend(); fig.tight_layout(); fig.savefig(output/"ratio.png"); plt.close(fig)
    fig, ax = plt.subplots()
    for algo in sorted({a for n,a in rates}):
        ns=sorted(n for n,a in rates if a==algo)
        ax.plot(ns,[statistics.median(rates[n,algo]) for n in ns],marker="o",label=algo)
    ax.set(xlabel="n",ylabel="GMAC/s"); ax.legend(); fig.tight_layout(); fig.savefig(output/"gmac.png"); plt.close(fig)
if __name__ == "__main__": main()
