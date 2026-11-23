# AI 维护：汇总独立进程 CSV；例子仅用于格式测试，不是实测。
# python scripts/summarize-packed-block.py INPUT_DIR
import csv, statistics, sys
from collections import defaultdict
from pathlib import Path

def main(root):
    groups=defaultdict(lambda: defaultdict(list))
    for path in Path(root).rglob("*.csv"):
        with path.open(newline="",encoding="utf-8-sig") as f:
            for row in csv.DictReader(f):
                algo=row["algorithm"]
                suffix="-"+algo
                if not path.parent.name.endswith(suffix): raise ValueError(f"unexpected process directory: {path.parent}")
                run=path.parent.name[:-len(suffix)]
                groups[(int(row["n"]),int(row["b"]))][(run,algo)].append(float(row["ms"]))
    if not groups: raise SystemExit("no data")
    for (n,b),data in sorted(groups.items()):
        med={(run,algo):statistics.median(values) for (run,algo),values in data.items()}
        runs=sorted({r for r,a in med if a=="blocked"} & {r for r,a in med if a=="blocked_packed"})
        ratios=[med[r,"blocked_packed"]/med[r,"blocked"] for r in runs]
        if len(ratios)!=10: print(n,b,"insufficient paired processes",len(ratios)); continue
        ratio=statistics.median(ratios); below=sum(x<1 for x in ratios); above=sum(x>1 for x in ratios)
        label="打包有效（启发式）" if ratio<=0.95 and below>=8 else "拷贝开销主导（启发式，原因待验证）" if ratio>=1.05 else "灰区：不追加调参"
        print(n,b,"median_ratio",ratio,"below",below,"above",above,label)
        print("规则 R 须 10/10 同侧且中位比偏离至少 5%；8/10 启发式不替代规则 R。")
        for algo in ("pack_only","blocked_packed","blocked"):
            values=[v for (r,a),v in med.items() if a==algo]
            print(algo,"median_ms",statistics.median(values) if values else "待测")
        print("pack_only 与整体耗时口径不同；差值不是直接测得的计算时间。")
if __name__=="__main__": main(sys.argv[1])
