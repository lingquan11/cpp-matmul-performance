#!/usr/bin/env python3
"""按函数统计汇编关键字，并拆开 vec-report 的行。

这是什么：读 .asm 里 PROC/ENDP 之间的 vpmulld、pmulld、ymm、xmm，
再读 vec-report 文本里的行号、是否向量化、原因码。只做关键字和原文拆行。
谁写的：脚手架由 AI 维护。表格最后一列留给零醛，脚本不填「是否向量化」的结论。
怎么运行：
    python3 scripts/scan-asm-keywords.py \\
        --asm tests/fixtures/asm-keyword-fixture/sample.asm \\
        --vec-report tests/fixtures/asm-keyword-fixture/sample-vec.txt
    python3 tests/test_scan_asm_keywords.py
限制：不编译、不计时、不改 day07。夹具是手写的，不是 cl 的输出。
vpmulld 先单独计数，剩下的 pmulld 才算 pmulld，避免一个指令算两次。
ymm / xmm 按词计数，ymm0 和 ymmword 各算一次。
没有 C5001/C5002 时只写「没有解析到」，不把空表说成未向量化。
本机 MSVC 19.51 是否真的打印这两条，以零醛机器上的 txt 为准。
Microsoft Learn 记载的是 info C5001 / info C5002：
https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/vectorizer-and-parallelizer-messages
"""

from __future__ import annotations

import argparse
import re
import sys
from dataclasses import dataclass, field
from pathlib import Path

PROC_RE = re.compile(r"^(\S+)\s+PROC\b", re.IGNORECASE)
ENDP_RE = re.compile(r"^(\S+)\s+ENDP\b", re.IGNORECASE)
KEYWORD_RE = {
    "vpmulld": re.compile(r"\bvpmulld\b", re.IGNORECASE),
    "pmulld": re.compile(r"\bpmulld\b", re.IGNORECASE),
    "ymm": re.compile(r"\bymm\w*", re.IGNORECASE),
    "xmm": re.compile(r"\bxmm\w*", re.IGNORECASE),
}
VEC_RE = re.compile(
    r"\((?P<line>\d+)\)\s*:\s*(?:info\s+C(?P<code>5001|5002)\s*:\s*)?(?P<body>.*)$",
    re.IGNORECASE,
)
REASON_RE = re.compile(
    r"reason(?:\s*:)?\s*['\"]?(?P<code>\d+)['\"]?",
    re.IGNORECASE,
)
SHORT_NAMES = (
    "matmul_ijk_lda",
    "matmul_ikj_lda",
    "matmul_blocked",
    "matmul_ikj",
    "matmul_ijk",
)


@dataclass
class FuncCount:
    symbol: str
    short: str
    counts: dict[str, int] = field(default_factory=dict)


@dataclass
class VecRow:
    line: int
    vectorized: str
    reason: str
    raw: str


def fail(message: str) -> None:
    print(message, file=sys.stderr)
    raise SystemExit(1)


def short_name(symbol: str) -> str:
    lowered = symbol.lower()
    for name in SHORT_NAMES:
        if name in lowered:
            return name
    return symbol


def count_keywords(text: str) -> dict[str, int]:
    # vpmulld 含有 pmulld 这六个字母。先抠掉整词，再数 pmulld。
    without_v = KEYWORD_RE["vpmulld"].sub(" ", text)
    return {
        "vpmulld": len(KEYWORD_RE["vpmulld"].findall(text)),
        "pmulld": len(KEYWORD_RE["pmulld"].findall(without_v)),
        "ymm": len(KEYWORD_RE["ymm"].findall(text)),
        "xmm": len(KEYWORD_RE["xmm"].findall(text)),
    }


def scan_asm(text: str) -> list[FuncCount]:
    sections: list[tuple[str, list[str]]] = []
    current_name: str | None = None
    current_lines: list[str] = []
    outside: list[str] = []
    for raw_line in text.splitlines():
        stripped = raw_line.strip()
        # 注释里的单词不算指令。夹具说明可以提到 vpmulld，不会进计数。
        if stripped.startswith(";") or stripped.startswith("//"):
            continue
        proc = PROC_RE.match(stripped)
        endp = ENDP_RE.match(stripped)
        if proc and current_name is None:
            current_name = proc.group(1)
            current_lines = []
            continue
        if endp and current_name is not None:
            sections.append((current_name, current_lines))
            current_name = None
            current_lines = []
            continue
        if current_name is None:
            outside.append(raw_line)
        else:
            current_lines.append(raw_line)
    if current_name is not None:
        sections.append((current_name, current_lines))

    counted = [
        FuncCount(symbol=name, short=short_name(name), counts=count_keywords("\n".join(lines)))
        for name, lines in sections
    ]
    outside_counts = count_keywords("\n".join(outside))
    if any(outside_counts.values()):
        counted.append(FuncCount("（函数外）", "（函数外）", outside_counts))
    return counted


def scan_vec(text: str) -> list[VecRow]:
    rows: list[VecRow] = []
    for raw_line in text.splitlines():
        stripped = raw_line.strip()
        if not stripped or stripped.startswith(";") or stripped.startswith("//"):
            continue
        matched = VEC_RE.search(stripped)
        if not matched:
            continue
        body = matched.group("body") or ""
        code = (matched.group("code") or "").upper()
        lowered = body.lower()
        vectorized: str | None = None
        if code == "5001" or (code == "" and "vectorized" in lowered and "not vectorized" not in lowered):
            vectorized = "是"
        elif code == "5002" or "not vectorized" in lowered:
            vectorized = "否"
        if vectorized is None:
            continue
        reason = ""
        if vectorized == "否":
            reason_match = REASON_RE.search(body)
            if reason_match:
                reason = reason_match.group("code")
        rows.append(
            VecRow(
                line=int(matched.group("line")),
                vectorized=vectorized,
                reason=reason,
                raw=stripped,
            )
        )
    return rows


def _cell(value: object) -> str:
    return str(value).replace("|", "\\|")


def render(functions: list[FuncCount], vec_rows: list[VecRow], labels: list[str]) -> str:
    lines = [
        "# 汇编关键字与 vec-report 拆行",
        "",
        "只做关键字计数和原文拆行。最后一列「我的判断」留给零醛，脚本留空。",
        "关键字出现不等于已经向量化，也不等于没有向量化。",
        "若报告显示未向量化，只把这一行留下来，不改源码去迎合编译器。",
        "",
        "输入：" + ("、".join(labels) if labels else "（无）"),
        "",
        "| 函数 | 汇编符号 | vpmulld | pmulld | ymm | xmm | 我的判断 |",
        "| --- | --- | ---: | ---: | ---: | ---: | --- |",
    ]
    if not functions:
        lines.append("| （没有 PROC/ENDP 分段） |  |  |  |  |  |  |")
    for item in functions:
        counts = item.counts
        lines.append(
            "| "
            + " | ".join(
                [
                    _cell(item.short),
                    _cell(item.symbol),
                    str(counts.get("vpmulld", 0)),
                    str(counts.get("pmulld", 0)),
                    str(counts.get("ymm", 0)),
                    str(counts.get("xmm", 0)),
                    "",
                ]
            )
            + " |"
        )
    lines.extend(
        [
            "",
            "| 源码行号 | 是否向量化 | 原因码 | 原文 |",
            "| --- | --- | --- | --- |",
        ]
    )
    if not vec_rows:
        lines.append("|  |  |  | 没有解析到 C5001/C5002 或 loop vectorized 行。以本机 txt 原文为准。 |")
    for row in vec_rows:
        lines.append(
            f"| {row.line} | {row.vectorized} | {row.reason} | {_cell(row.raw)} |"
        )
    lines.append("")
    return "\n".join(lines)


def main(argv: list[str] | None = None) -> None:
    parser = argparse.ArgumentParser(description="统计汇编关键字并拆 vec-report")
    parser.add_argument("--asm", action="append", default=[], help="汇编文件，可重复")
    parser.add_argument("--vec-report", action="append", default=[], help="vec-report 文本，可重复")
    args = parser.parse_args(argv)
    if not args.asm and not args.vec_report:
        fail("至少给一个 --asm 或 --vec-report")

    functions: list[FuncCount] = []
    vec_rows: list[VecRow] = []
    labels: list[str] = []
    for raw_path in args.asm:
        path = Path(raw_path)
        if not path.is_file():
            fail(f"找不到汇编：{path}")
        functions.extend(scan_asm(path.read_text(encoding="utf-8", errors="replace")))
        labels.append(path.as_posix())
    for raw_path in args.vec_report:
        path = Path(raw_path)
        if not path.is_file():
            fail(f"找不到 vec-report：{path}")
        vec_rows.extend(scan_vec(path.read_text(encoding="utf-8", errors="replace")))
        labels.append(path.as_posix())
    sys.stdout.write(render(functions, vec_rows, labels))


if __name__ == "__main__":
    main()
