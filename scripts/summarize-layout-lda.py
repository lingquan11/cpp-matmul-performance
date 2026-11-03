#!/usr/bin/env python3
"""汇总行跨度实验的进程中位数，并按提示词里的阈值给判定词。

这是什么：只读 layout-lda.csv。每个进程先取本文件 ms_per_multiply 的中位数，
再比 n=256、ijk、lda=272 与 lda=256。同时看 ikj 在各 LDA 上的相对极差。
谁写的：脚手架由 AI 维护。阈值来自任务说明，不是我测出来的耗时。
怎么运行：
    python3 scripts/summarize-layout-lda.py <结果目录>
    python3 tests/test_summarize_layout_lda.py
限制：不运行 exe，不写 CSV。测试夹具里的数字是格式用的，非实测，
不能抄进正式报告或 metadata。正式计时由零醛在 Windows 本机运行。

口径：奇数个样本取排序后正中间那个；偶数个取正中间两个的算术平均。
这与尚未合并的分支 ideas/matmul-1002-i1-measurement-baseline 里
experiments/reports/measurement-rules.md 的中位数算法相同。本脚本不复制那份草案。
规则 R 自己的门槛是「偏离 1 至少 5%」。这里不用那条 5%。
本实验的词是：
- 中位比 = median(lda=272) / median(lda=256) <= 0.90，且 10 个进程的
  配对比都小于 1：行跨度显著影响
- 中位比落在 0.97 到 1.03（含端点）：无影响
- 其余：灰区
- 进程数不是 10：不判
配对按 run 编号，不按耗时排序。大于 1 的一侧不记成「行跨度显著影响」，
因为提示词只给了 <=0.90 这一档。
ikj：每个 n 上，各 LDA 的进程中位数再取中位数，(最大-最小)/最小 <= 0.03
记「<=3%」，否则记「超过 3%」。这是检查，不是另一条性能结论。
"""

from __future__ import annotations

import csv
import re
import sys
from decimal import Decimal
from pathlib import Path

CSV_NAME = "layout-lda.csv"
COLUMNS = (
    "n",
    "lda",
    "algorithm",
    "pilot_ms",
    "sample",
    "repeats",
    "total_ms",
    "ms_per_multiply",
    "C00",
    "expected_C00",
    "checksum",
    "elementwise_match",
    "status",
)
RATIO_FAST = Decimal("0.90")
RATIO_LOW = Decimal("0.97")
RATIO_HIGH = Decimal("1.03")
IKJ_SPAN = Decimal("0.03")
PROCESS_COUNT = 10
RUN_RE = re.compile(r"^run(\d+)$")


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


def median(values: list[Decimal]) -> Decimal:
    if not values:
        raise ValueError("空列表没有中位数")
    ordered = sorted(values)
    mid = len(ordered) // 2
    if len(ordered) % 2 == 1:
        return ordered[mid]
    return (ordered[mid - 1] + ordered[mid]) / 2


def judge_row_stride(ratio: Decimal, process_ratios: list[Decimal]) -> str:
    if len(process_ratios) != PROCESS_COUNT:
        return "不判"
    if ratio <= RATIO_FAST and all(item < 1 for item in process_ratios):
        return "行跨度显著影响"
    if RATIO_LOW <= ratio <= RATIO_HIGH:
        return "无影响"
    return "灰区"


def ikj_span(level_medians: list[Decimal]) -> Decimal | None:
    if len(level_medians) < 2:
        return None
    low = min(level_medians)
    high = max(level_medians)
    if low <= 0:
        fail("ikj 的分母不是正数")
    return (high - low) / low


def ikj_label(span: Decimal | None) -> str:
    if span is None:
        return "LDA 水平不足，不比较"
    if span <= IKJ_SPAN:
        return "<=3%"
    return "超过 3%"


def _positive(text: str, path: Path, line_no: int) -> Decimal:
    try:
        value = Decimal(text)
    except Exception:
        fail(f"{path} 第 {line_no} 行 ms_per_multiply 不是数字：{text}")
    if value <= 0:
        fail(f"{path} 第 {line_no} 行 ms_per_multiply 不是正数：{text}")
    return value


def load_process(path: Path) -> tuple[str, str, str, str, Decimal]:
    run_match = RUN_RE.match(path.parent.name)
    if not run_match:
        fail(f"{path} 的上一层目录不是 runNN")
    run_id = str(int(run_match.group(1)))
    with path.open(newline="", encoding="utf-8") as handle:
        reader = csv.DictReader(handle)
        missing = set(COLUMNS).difference(reader.fieldnames or ())
        if missing:
            fail(f"{path} 缺少列：{', '.join(sorted(missing))}")
        rows = list(reader)
    if not rows:
        fail(f"{path} 没有数据行")
    samples: dict[str, Decimal] = {}
    n = rows[0]["n"]
    lda = rows[0]["lda"]
    algorithm = rows[0]["algorithm"]
    for index, row in enumerate(rows, start=2):
        if row["n"] != n or row["lda"] != lda or row["algorithm"] != algorithm:
            fail(f"{path} 第 {index} 行和第一行的 n/lda/algorithm 不一致")
        if row["status"] != "PASS" or row["elementwise_match"] != "YES":
            fail(f"{path} 第 {index} 行不是 PASS/YES")
        sample = row["sample"]
        if sample in samples:
            fail(f"{path} 的 sample {sample} 重复")
        samples[sample] = _positive(row["ms_per_multiply"], path, index)
    return n, lda, algorithm, run_id, median(list(samples.values()))


def load_tree(root: Path) -> dict[tuple[str, str, str], dict[str, Decimal]]:
    if not root.is_dir():
        fail(f"找不到目录：{root}")
    grouped: dict[tuple[str, str, str], dict[str, Decimal]] = {}
    paths = sorted(root.rglob(CSV_NAME))
    if not paths:
        fail(f"{root} 下没有 {CSV_NAME}")
    for path in paths:
        n, lda, algorithm, run_id, value = load_process(path)
        key = (n, lda, algorithm)
        bucket = grouped.setdefault(key, {})
        if run_id in bucket:
            fail(f"{root} 里 {key} 的 run{int(run_id):02d} 出现了两次")
        bucket[run_id] = value
    return grouped


def _paired_ratio(
    fast: dict[str, Decimal],
    slow: dict[str, Decimal],
    label: str,
) -> tuple[Decimal, list[Decimal]]:
    if set(fast) != set(slow):
        fail(f"{label} 的 run 编号对不齐：{sorted(fast)} 对 {sorted(slow)}")
    run_ids = sorted(fast, key=int)
    ratios = [fast[run_id] / slow[run_id] for run_id in run_ids]
    return median(list(fast.values())) / median(list(slow.values())), ratios


def render(root: Path, grouped: dict[tuple[str, str, str], dict[str, Decimal]]) -> str:
    lines = [
        "# 行跨度 LDA 汇总",
        "",
        f"目录：{root}",
        "每个进程先取 ms_per_multiply 的中位数，再按 run 编号配对。",
        "计时依赖规则 R 的进程中位数口径。规则全文在尚未合并的分支",
        "ideas/matmul-1002-i1-measurement-baseline 的 experiments/reports/measurement-rules.md。",
        "本输出的 0.90 / 0.97–1.03 是行跨度实验自己的阈值，不是规则 R 的 5%。",
        "下面的数字只来自读到的 CSV。夹具目录里的数不是实测。",
        "",
    ]
    ijk_256 = grouped.get(("256", "272", "ijk"))
    ijk_base = grouped.get(("256", "256", "ijk"))
    if ijk_256 is None or ijk_base is None:
        lines.append("n=256 的 ijk 缺少 lda=272 或 lda=256，不判。")
        lines.append("")
    else:
        ratio, process_ratios = _paired_ratio(ijk_256, ijk_base, "n=256 ijk lda=272/256")
        verdict = judge_row_stride(ratio, process_ratios)
        lines.append(f"n=256 ijk 中位比（lda=272 / lda=256）= {ratio}")
        lines.append(f"进程数 = {len(process_ratios)}")
        lines.append("进程配对比 = " + ", ".join(str(item) for item in process_ratios))
        lines.append(f"判定 = {verdict}")
        lines.append("")

    seen_n: list[str] = []
    for n, _lda, algorithm in grouped:
        if algorithm == "ikj" and n not in seen_n:
            seen_n.append(n)
    if not seen_n:
        lines.append("没有 ikj 行，不比较 ikj 随 LDA 的变化。")
    for n in sorted(seen_n, key=int):
        levels: list[Decimal] = []
        labels: list[str] = []
        for (size, lda, algorithm), runs in sorted(grouped.items(), key=lambda item: int(item[0][1])):
            if size == n and algorithm == "ikj":
                labels.append(lda)
                levels.append(median(list(runs.values())))
        span = ikj_span(levels)
        span_text = "不适用" if span is None else str(span)
        lines.append(
            f"ikj n={n} LDA={','.join(labels)} 相对极差={span_text} 变化={ikj_label(span)}"
        )
    lines.append("")
    present_sizes = sorted({key[0] for key in grouped}, key=int)
    lines.append("读到的 n = " + ", ".join(present_sizes))
    lines.append("没有出现在 CSV 里的规模保持空白，不补数。")
    lines.append("")
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> None:
    args = list(sys.argv[1:] if argv is None else argv)
    if len(args) != 1:
        fail("用法: python3 scripts/summarize-layout-lda.py <结果目录>")
    root = Path(args[0])
    sys.stdout.write(render(root, load_tree(root)))


if __name__ == "__main__":
    main()
