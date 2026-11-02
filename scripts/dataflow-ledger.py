#!/usr/bin/env python3
"""OS / WS / IS 数据流访存账本的脚手架。

它是什么：解析 --M --K --N --R --C --dataflow，在计数实现之后打印 A/B/C
读写次数表，并可与手推闭式总数对照。
谁写的：脚手架由 AI 维护。count_os、count_ws、count_is 标 TODO(零醛)，
由零醛本人实现，不是 AI 代写的学生实现。
怎么运行：
    python3 scripts/dataflow-ledger.py --M 64 --K 64 --N 64 --R 8 --C 8 --dataflow OS
限制：三个计数函数未实现。未实现时向 stderr 写「未实现」，退出码为 1，
不打印访问次数。本脚本不含 SCALE-Sim，也不编造次数。

参数含义（形状，不是测量结果）：A 为 M×K，B 为 K×N，C 为 M×N。
R、C 是阵列的行数和列数，与命令行参数同名。行列方向若与论文相反，待零醛确认。

计数口径（待零醛确认）：
- C 的部分和，第一趟是否计读？
- 只计片外/缓冲访问，还是也计 PE 间传递？
未确认前不要在三个 count_* 里写公式。

tile：若 matmul-0403-I5 先合并，划分沿用 ⌈n/P⌉ 与边缘补零。
本仓库还没有那份计数器。这里只记下约定，不实现公式。
"""

from __future__ import annotations

import argparse
import sys
from typing import Mapping

COUNT_KEYS = (
    "A_reads",
    "A_writes",
    "B_reads",
    "B_writes",
    "C_reads",
    "C_writes",
)


def count_os(M: int, K: int, N: int, R: int, C: int) -> dict[str, int]:
    """输出驻留（OS）的 A/B/C 读写次数。

    计数口径（待零醛确认）：C 的部分和第一趟是否计读？
    只计片外/缓冲访问，还是也计 PE 间传递？

    实现后返回 A_reads、A_writes、B_reads、B_writes、C_reads、C_writes。
    未实现。不要在这里写闭式公式。
    """
    del M, K, N, R, C
    raise NotImplementedError("TODO(零醛)")


def count_ws(M: int, K: int, N: int, R: int, C: int) -> dict[str, int]:
    """权重驻留（WS）的 A/B/C 读写次数。

    计数口径（待零醛确认）：C 的部分和第一趟是否计读？
    只计片外/缓冲访问，还是也计 PE 间传递？
    WS 的次数取决于这一口径。

    未实现。不要在这里写闭式公式。
    """
    del M, K, N, R, C
    raise NotImplementedError("TODO(零醛)")


def count_is(M: int, K: int, N: int, R: int, C: int) -> dict[str, int]:
    """输入驻留（IS）的 A/B/C 读写次数。

    计数口径（待零醛确认）：C 的部分和第一趟是否计读？
    只计片外/缓冲访问，还是也计 PE 间传递？
    IS 的次数取决于这一口径。

    未实现。不要在这里写闭式公式。
    """
    del M, K, N, R, C
    raise NotImplementedError("TODO(零醛)")


def total_accesses(counts: Mapping[str, int]) -> int:
    """把六项读写加总。这是表格用的加法，不是数据流公式。"""
    missing = [key for key in COUNT_KEYS if key not in counts]
    if missing:
        raise ValueError("计数结果缺字段：" + ",".join(missing))
    total = 0
    for key in COUNT_KEYS:
        value = counts[key]
        if isinstance(value, bool) or not isinstance(value, int):
            raise ValueError(f"{key} 不是整数")
        if value < 0:
            raise ValueError(f"{key} 为负")
        total += value
    return total


def compare_closed_form(computed_total: int, closed_form_total: int) -> str:
    """手推闭式总数和程序总数是否相同。闭式值由调用方传入，本函数不保存参考值。"""
    if computed_total == closed_form_total:
        return "MATCH"
    return "MISMATCH"


def render_table(
    dataflow: str,
    M: int,
    K: int,
    N: int,
    R: int,
    C: int,
    counts: Mapping[str, int],
) -> str:
    total = total_accesses(counts)
    lines = [
        f"dataflow {dataflow}",
        f"M {M} K {K} N {N} R {R} C {C}",
        "tensor reads writes",
    ]
    for tensor in ("A", "B", "C"):
        lines.append(
            f"{tensor} {counts[tensor + '_reads']} {counts[tensor + '_writes']}"
        )
    lines.append(f"total {total}")
    return "\n".join(lines)


def positive_int(text: str) -> int:
    if not text.isdigit():
        raise argparse.ArgumentTypeError("需要十进制正整数")
    value = int(text)
    if value <= 0:
        raise argparse.ArgumentTypeError("需要正整数")
    return value


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        description=(
            "数据流访存账本脚手架。核心计数未实现。"
            "计数口径（待零醛确认）：C 的部分和第一趟是否计读？"
            "只计片外/缓冲访问，还是也计 PE 间传递？"
        )
    )
    parser.add_argument("--M", type=positive_int, required=True)
    parser.add_argument("--K", type=positive_int, required=True)
    parser.add_argument("--N", type=positive_int, required=True)
    parser.add_argument("--R", type=positive_int, required=True, help="阵列行数")
    parser.add_argument("--C", type=positive_int, required=True, help="阵列列数")
    parser.add_argument("--dataflow", choices=("OS", "WS", "IS"), required=True)
    parser.add_argument(
        "--closed-form-total",
        type=positive_int,
        default=None,
        help="手推闭式总数。程序不内置该数。未实现时不会拿它冒充输出。",
    )
    return parser


def count_for(dataflow: str, M: int, K: int, N: int, R: int, C: int) -> dict[str, int]:
    if dataflow == "OS":
        return count_os(M, K, N, R, C)
    if dataflow == "WS":
        return count_ws(M, K, N, R, C)
    if dataflow == "IS":
        return count_is(M, K, N, R, C)
    raise ValueError(f"未知 dataflow：{dataflow}")


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)
    try:
        counts = count_for(args.dataflow, args.M, args.K, args.N, args.R, args.C)
    except NotImplementedError as exc:
        print(f"未实现：{args.dataflow} {exc}", file=sys.stderr)
        print("未输出访问次数。", file=sys.stderr)
        return 1
    try:
        text = render_table(args.dataflow, args.M, args.K, args.N, args.R, args.C, counts)
        computed = total_accesses(counts)
    except ValueError as exc:
        print(f"计数结果不能制表：{exc}", file=sys.stderr)
        return 1
    print(text)
    if args.closed_form_total is not None:
        verdict = compare_closed_form(computed, args.closed_form_total)
        print(f"closed_form_total {args.closed_form_total}")
        print(f"compare {verdict}")
        if verdict != "MATCH":
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
