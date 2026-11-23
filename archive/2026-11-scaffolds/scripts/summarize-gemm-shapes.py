# AI 维护：按进程中位 GMAC/s 分组；fixture 非实测。
# python scripts/summarize-gemm-shapes.py RESULTS_DIR
import csv, statistics, sys
from pathlib import Path
from collections import defaultdict
def main(root):
    data=defaultdict(list)
    for p in Path(root).rglob("*.csv"):
        run,shape=p.parent.name.split("-",1)
        with p.open(newline="",encoding="utf-8-sig") as f:
            for row in csv.DictReader(f): data[run,shape].append(float(row["gmac_per_s"]))
    expected={"square","tall","wide","deep","shortk"}
    runs=sorted({r for r,s in data})
    if len(runs)!=10 or any({s for r,s in data if r==run}!=expected for run in runs): raise SystemExit("need ten complete process rounds")
    ranks=[]; all_values=defaultdict(list)
    for run in runs:
        med={s:statistics.median(data[run,s]) for s in expected}
        if min(med.values())<=0 or len(set(med.values()))!=len(med): raise SystemExit("nonpositive or tied throughput: cannot rank")
        ranks.append(tuple(sorted(med,key=med.get)))
        for shape,v in med.items(): all_values[shape].append(v)
    overall={s:statistics.median(v) for s,v in all_values.items()}
    ratio=max(overall.values())/min(overall.values())
    print("max/min median GMAC/s:",ratio,"stable ranking:",len(set(ranks))==1)
    print("shape sensitivity established" if ratio>1.2 and len(set(ranks))==1 else "inconclusive")
if __name__=="__main__": main(sys.argv[1])
