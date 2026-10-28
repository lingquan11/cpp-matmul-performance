#!/usr/bin/env python3
"""从已归档的 Day 5、Day 7 程序导出 CSV 打印中位数表。

只读。不写 CSV，不运行计时程序。
每组必须是 5 条。中位数是排序后的第 3 个 ms_per_multiply。
比值写到两位小数，相对变化写到一位小数，四舍五入。
"""

from __future__ import annotations

import csv
import sys
from decimal import Decimal, ROUND_HALF_UP
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
DAY5_CSV = (
    ROOT
    / "experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv"
)
DAY7_CSV = (
    ROOT
    / "experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv"
)

SIZES = ("64", "128", "256")
DAY5_ALGORITHMS = ("ijk", "ikj")
DAY7_ALGORITHMS = ("ijk", "ikj", "blocked_16", "blocked_32", "blocked_64")


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


def rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_rows(path: Path) -> list[dict[str, str]]:
    if not path.is_file():
        fail(f"找不到 CSV：{rel(path)}")
    with path.open(newline="") as handle:
        reader = csv.DictReader(handle)
        required = {"n", "algorithm", "ms_per_multiply", "status"}
        missing = required.difference(reader.fieldnames or ())
        if missing:
            fail(f"{rel(path)} 缺少列：{', '.join(sorted(missing))}")
        rows = list(reader)
    if not rows:
        fail(f"{rel(path)} 没有数据行")
    return rows


def group_samples(
    rows: list[dict[str, str]],
    path: Path,
    algorithms: tuple[str, ...],
) -> dict[tuple[str, str], list[Decimal]]:
    expected = {(size, algorithm) for size in SIZES for algorithm in algorithms}
    grouped: dict[tuple[str, str], list[Decimal]] = {key: [] for key in expected}
    for index, row in enumerate(rows, start=2):
        key = (row["n"], row["algorithm"])
        if key not in grouped:
            fail(
                f"{rel(path)} 第 {index} 行的 n/algorithm 不在报告表里：{key[0]} {key[1]}"
            )
        if row["status"] != "PASS":
            fail(f"{rel(path)} 第 {index} 行 status 不是 PASS：{row['status']}")
        try:
            value = Decimal(row["ms_per_multiply"])
        except Exception:
            fail(
                f"{rel(path)} 第 {index} 行 ms_per_multiply 不是数字：{row['ms_per_multiply']}"
            )
        grouped[key].append(value)
    for key, values in grouped.items():
        if len(values) != 5:
            fail(
                f"{rel(path)} 的 {key[0]} {key[1]} 应有 5 条，实际 {len(values)} 条"
            )
    return grouped


def median_of_five(values: list[Decimal]) -> Decimal:
    return sorted(values)[2]


def ms_text(value: Decimal) -> str:
    return f"{value.quantize(Decimal('0.000001')):f}"


def ratio_text(numerator: Decimal, denominator: Decimal) -> str:
    ratio = (numerator / denominator).quantize(Decimal("0.01"), rounding=ROUND_HALF_UP)
    return f"{ratio}×"


def percent_text(block: Decimal, baseline: Decimal) -> str:
    percent = ((block - baseline) / baseline * Decimal(100)).quantize(
        Decimal("0.1"), rounding=ROUND_HALF_UP
    )
    sign = "+" if percent > 0 else ""
    return f"{sign}{percent}%"


def print_day5(grouped: dict[tuple[str, str], list[Decimal]]) -> None:
    print("## Day 5 中位数（ms_per_multiply）")
    print(f"来源：{rel(DAY5_CSV)}")
    print("与 experiments/reports/2026-10-26-day05-loop-order.md 的中位数表对应。")
    print()
    print("| n | ijk 中位数 (ms) | ikj 中位数 (ms) | 中位数耗时比（ijk/ikj） |")
    print("| ---: | ---: | ---: | ---: |")
    for size in SIZES:
        ijk = median_of_five(grouped[(size, "ijk")])
        ikj = median_of_five(grouped[(size, "ikj")])
        print(f"| {size} | {ms_text(ijk)} | {ms_text(ikj)} | {ratio_text(ijk, ikj)} |")
    print()


def print_day7(grouped: dict[tuple[str, str], list[Decimal]]) -> None:
    medians = {
        (size, algorithm): median_of_five(grouped[(size, algorithm)])
        for size in SIZES
        for algorithm in DAY7_ALGORITHMS
    }
    print("## Day 7 中位数（ms_per_multiply）")
    print(f"来源：{rel(DAY7_CSV)}")
    print("与 experiments/reports/2026-10-27-day07-blocking.md 的中位数表对应。")
    print()
    print("| n | ijk | ikj | blocked_16 | blocked_32 | blocked_64 |")
    print("|---:|---:|---:|---:|---:|---:|")
    for size in SIZES:
        cells = " | ".join(ms_text(medians[(size, algorithm)]) for algorithm in DAY7_ALGORITHMS)
        print(f"| {size} | {cells} |")
    print()
    print("## Day 7：ijk 相对 ikj 的中位数比")
    print()
    print("| n | ijk 中位数 / ikj 中位数 |")
    print("|---:|---:|")
    for size in SIZES:
        print(
            f"| {size} | {ratio_text(medians[(size, 'ijk')], medians[(size, 'ikj')])} |"
        )
    print()
    print("## Day 7：分块相对 ikj 的中位数耗时变化")
    print("正数代表分块更慢，负数代表更快。")
    print()
    print("| n | blocked_16 | blocked_32 | blocked_64 |")
    print("|---:|---:|---:|---:|")
    for size in SIZES:
        baseline = medians[(size, "ikj")]
        cells = " | ".join(
            percent_text(medians[(size, algorithm)], baseline)
            for algorithm in ("blocked_16", "blocked_32", "blocked_64")
        )
        print(f"| {size} | {cells} |")
    print()


def main() -> None:
    if len(sys.argv) != 1:
        print("用法: python3 experiments/summarize_medians.py", file=sys.stderr)
        print("没有参数。只读归档 CSV，打印中位数，不写文件。", file=sys.stderr)
        raise SystemExit(2)

    print("只读归档 CSV，打印中位数。不写文件，不运行计时程序。计时复现：未验证。")
    print()
    day5 = group_samples(load_rows(DAY5_CSV), DAY5_CSV, DAY5_ALGORITHMS)
    day7 = group_samples(load_rows(DAY7_CSV), DAY7_CSV, DAY7_ALGORITHMS)
    print_day5(day5)
    print_day7(day7)


if __name__ == "__main__":
    main()
