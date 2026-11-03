# AI 维护：每轮独立进程的配对统计；这是启发式，不是硬件停顿周期。
# python scripts/summarize-planb.py RESULTS_DIR
import csv, statistics, sys
from pathlib import Path
from collections import defaultdict
def main(root):
    data=defaultdict(dict)
    for p in Path(root).rglob("*.csv"):
        with p.open(newline="",encoding="utf-8-sig") as f:
            for row in csv.DictReader(f):
                suffix="-"+row["variant"]
                if not p.parent.name.endswith(suffix): raise ValueError("bad directory name")
                pair=p.parent.name[:-len(suffix)]
                data[(row["algo"],int(row["n"]))][pair,row["variant"]]=float(row["ms_per_multiply"])
    if not data: raise SystemExit("no data")
    print("数据重用热度时间差（启发式），不是硬件访存停顿周期或缺失次数")
    for key,values in sorted(data.items()):
        pairs=sorted({p for p,v in values})
        if len(pairs)!=10 or any((p,"real") not in values or (p,"resident") not in values for p in pairs): raise SystemExit("need ten complete pairs")
        if any(values[p,"real"]<=0 or values[p,"resident"]<=0 for p in pairs): raise ValueError("time must be positive")
        ratios=[values[p,"resident"]/values[p,"real"] for p in pairs]
        delta=statistics.median([1-r for r in ratios]); median=statistics.median(ratios)
        direction=(all(r<1 for r in ratios) or all(r>1 for r in ratios)) and abs(median-1)>=0.05
        print(key,"median_delta",delta,"rule_R", "same side, >=5%" if direction else "inconclusive")
if __name__=="__main__": main(sys.argv[1])
