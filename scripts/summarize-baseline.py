#!/usr/bin/env python3
"""测量地基：只读汇总 day07 的跨进程 CSV，并按规则 R 判定。

这是什么：读结果目录里的 matmul_benchmark_day7_blocking.csv，对每个条件、
每个 (算法, n) 算跨进程的中位数、CV、(max-min)/median，再判定 blocked_64
相对 ikj 是快、慢还是「不判」。
谁写的：脚手架由 AI 维护。规则阈值待零醛确认，不是学生实现的核心算法。
怎么运行：在仓库根目录执行
    python3 scripts/summarize-baseline.py <结果目录>
    python3 tests/test_summarize_baseline.py
限制：不运行 exe，不写 CSV，不产生计时。tests/fixtures/baseline-fixture/
里的数字是格式测试用，非实测，不能抄进正式报告或 metadata。
正式计时由零醛在 Windows 本机运行。规则全文见
experiments/reports/measurement-rules.md。

口径：每个进程先对 5 条 ms_per_multiply 取中位数（5 是奇数，取排序后的
第 3 个）。进程数是偶数时，跨进程中位数取排序后正中间两个的算术平均。
规则 R 看的是 10 个进程的 blocked_64/ikj，不是单个进程里的 5 个批次，
也不是 C0 对 C1。CV 用样本标准差（除以 N-1）除以算术平均。N<2 时 CV
和 (max-min)/median 写「不适用」，不能当成噪声是 0。CV>=3% 只打标签。
"""

from __future__ import annotations

import csv
import sys
from dataclasses import dataclass
from decimal import Decimal, ROUND_HALF_UP
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
CSV_NAME = "matmul_benchmark_day7_blocking.csv"
COLUMNS = (
    "n",
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
SIZES = ("64", "128", "256")
ALGORITHMS = ("ijk", "ikj", "blocked_16", "blocked_32", "blocked_64")
EXPECTED_KEYS = tuple((size, algorithm) for size in SIZES for algorithm in ALGORITHMS)
CV_LIMIT = Decimal("0.03")
DEVIATION_LIMIT = Decimal("0.05")
RULE_PROCESS_COUNT = 10
NOISE_TAG = "环境噪声偏高"


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


def sample_cv(values: list[Decimal]) -> Decimal | None:
    if len(values) < 2:
        return None
    count = Decimal(len(values))
    mean = sum(values, Decimal(0)) / count
    if mean <= 0:
        fail("CV 的分母不是正数，无法计算")
    variance = sum((value - mean) ** 2 for value in values) / (count - 1)
    return variance.sqrt() / mean


def noise_label(cv: Decimal | None) -> str:
    if cv is not None and cv >= CV_LIMIT:
        return NOISE_TAG
    return ""


def spread_ratio(values: list[Decimal]) -> Decimal | None:
    if len(values) < 2:
        return None
    middle = median(values)
    if middle <= 0:
        fail("(max-min)/median 的分母不是正数")
    return (max(values) - min(values)) / middle


@dataclass(frozen=True)
class RuleDecision:
    verdict: str
    reason: str
    median_ratio: Decimal | None


def judge_rule_r(ratios: list[Decimal]) -> RuleDecision:
    """10/10 在 1 的同一侧，且中位比偏离 1 至少 5%，才写快或慢。"""
    if not ratios:
        return RuleDecision("不判", "没有比值，规则 R 不适用", None)
    median_ratio = median(ratios)
    if len(ratios) != RULE_PROCESS_COUNT:
        return RuleDecision(
            "不判",
            f"进程数是 {len(ratios)}，不是 {RULE_PROCESS_COUNT}，规则 R 不适用",
            median_ratio,
        )
    above = sum(1 for ratio in ratios if ratio > 1)
    below = sum(1 for ratio in ratios if ratio < 1)
    deviation = abs(median_ratio - 1)
    if above == RULE_PROCESS_COUNT or below == RULE_PROCESS_COUNT:
        if deviation >= DEVIATION_LIMIT:
            verdict = "慢" if above == RULE_PROCESS_COUNT else "快"
            return RuleDecision(
                verdict,
                "10/10 落在 1 的同一侧，且中位比偏离 1 至少 5%",
                median_ratio,
            )
        return RuleDecision(
            "不判",
            "10/10 落在 1 的同一侧，但中位比偏离 1 不足 5%",
            median_ratio,
        )
    same_side = max(above, below)
    return RuleDecision(
        "不判",
        f"不是 10/10 落在 1 的同一侧（{same_side}/10 在一侧）",
        median_ratio,
    )


@dataclass(frozen=True)
class AlgorithmStats:
    n: str
    algorithm: str
    process_medians: tuple[Decimal, ...]
    median: Decimal
    cv: Decimal | None
    spread: Decimal | None
    noise_tag: str


@dataclass(frozen=True)
class Judgment:
    n: str
    ratios: tuple[Decimal, ...]
    decision: RuleDecision
    noise_tag: str


@dataclass(frozen=True)
class ConditionSummary:
    name: str
    path: Path
    kind: str
    run_names: tuple[str, ...]
    algorithms: tuple[AlgorithmStats, ...]
    judgments: tuple[Judgment, ...]


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def load_process_medians(path: Path) -> dict[tuple[str, str], Decimal]:
    try:
        handle = path.open(newline="", encoding="utf-8-sig")
    except OSError as exc:
        fail(f"无法读取 {display_path(path)}：{exc}")
    with handle:
        reader = csv.DictReader(handle)
        if list(reader.fieldnames or []) != list(COLUMNS):
            fail(
                f"{display_path(path)} 的表头不是 day07 的 12 列。"
                f"实际：{reader.fieldnames}"
            )
        try:
            rows = list(reader)
        except csv.Error as exc:
            fail(f"{display_path(path)} 读 CSV 失败：{exc}")
    if len(rows) != 75:
        fail(f"{display_path(path)} 应有 75 行数据，实际 {len(rows)} 行")

    grouped: dict[tuple[str, str], dict[int, Decimal]] = {
        key: {} for key in EXPECTED_KEYS
    }
    for index, row in enumerate(rows, start=2):
        key = (row["n"], row["algorithm"])
        if key not in grouped:
            fail(f"{display_path(path)} 第 {index} 行的 n/algorithm 不在 day07 表里：{key}")
        if row["status"] != "PASS" or row["elementwise_match"] != "YES":
            fail(f"{display_path(path)} 第 {index} 行不是 PASS/YES，不拿来做计时汇总")
        try:
            sample = int(row["sample"])
            repeats = int(row["repeats"])
            per = Decimal(row["ms_per_multiply"])
            total = Decimal(row["total_ms"])
            pilot = Decimal(row["pilot_ms"])
            c00 = int(row["C00"])
            expected = int(row["expected_C00"])
            int(row["checksum"])
        except Exception:
            fail(f"{display_path(path)} 第 {index} 行有无法解析的数字")
        if sample in grouped[key]:
            fail(f"{display_path(path)} 的 {key} 重复了 sample {sample}")
        if sample not in range(1, 6):
            fail(f"{display_path(path)} 第 {index} 行 sample 不是 1 到 5：{sample}")
        if repeats <= 0 or per <= 0 or total <= 0 or pilot <= 0:
            fail(f"{display_path(path)} 第 {index} 行的耗时或 repeats 不是正数")
        if abs(total / Decimal(repeats) - per) > Decimal("0.000001"):
            fail(f"{display_path(path)} 第 {index} 行 total_ms / repeats 对不上 ms_per_multiply")
        if c00 != expected:
            fail(f"{display_path(path)} 第 {index} 行 C00 和 expected_C00 不一致")
        grouped[key][sample] = per

    medians: dict[tuple[str, str], Decimal] = {}
    for key, samples in grouped.items():
        if set(samples) != {1, 2, 3, 4, 5}:
            fail(f"{display_path(path)} 的 {key[0]} {key[1]} 没有正好 5 条")
        medians[key] = median([samples[sample] for sample in range(1, 6)])
    return medians


def discover(root: Path) -> list[tuple[str, Path, str, list[Path]]]:
    if not root.is_dir():
        fail(f"不是目录：{display_path(root)}")

    def run_dirs(condition: Path) -> list[Path]:
        found = []
        for child in condition.iterdir():
            if not child.is_dir() or child.name.startswith("."):
                continue
            if _run_number(child.name) is None:
                continue
            found.append(child)
        if not found:
            return []
        missing = [child for child in found if not (child / CSV_NAME).is_file()]
        if missing:
            fail(f"{display_path(missing[0])} 缺少 {CSV_NAME}，不跳过半截运行")
        return sorted(found, key=lambda item: _run_number(item.name) or 0)

    def kind_of(runs: list[Path]) -> str:
        kinds = {name_kind(path.name) for path in runs}
        if len(kinds) != 1:
            fail(f"{display_path(runs[0].parent)} 里混放了 runNN 和 coreNN")
        return kinds.pop()

    direct = run_dirs(root)
    if direct:
        return [(root.name, root, kind_of(direct), direct)]

    found: list[tuple[str, Path, str, list[Path]]] = []
    for child in sorted(root.iterdir(), key=lambda item: item.name):
        if not child.is_dir() or child.name.startswith("."):
            continue
        runs = run_dirs(child)
        if runs:
            found.append((child.name, child, kind_of(runs), runs))
    if not found:
        fail(
            f"{display_path(root)} 里没有 runNN/ 或 coreNN/ 下的 {CSV_NAME}。"
            "空模板不会被编造成测量结果。"
        )
    return found


def _run_number(name: str) -> int | None:
    if name.startswith("run") and name[3:].isdigit():
        return int(name[3:])
    if name.startswith("core") and name[4:].isdigit():
        return int(name[4:])
    return None


def name_kind(name: str) -> str:
    if name.startswith("core"):
        return "core"
    if name.startswith("run"):
        return "run"
    fail(f"无法识别的运行目录名：{name}")
    return ""


def summarize_condition(
    name: str,
    path: Path,
    kind: str,
    runs: list[Path],
) -> ConditionSummary:
    loaded = [(run.name, load_process_medians(run / CSV_NAME)) for run in runs]
    algorithms: list[AlgorithmStats] = []
    for size, algorithm in EXPECTED_KEYS:
        process_medians = tuple(medians[(size, algorithm)] for _, medians in loaded)
        cv = sample_cv(list(process_medians))
        algorithms.append(
            AlgorithmStats(
                n=size,
                algorithm=algorithm,
                process_medians=process_medians,
                median=median(list(process_medians)),
                cv=cv,
                spread=spread_ratio(list(process_medians)),
                noise_tag=noise_label(cv),
            )
        )
    by_key = {(item.n, item.algorithm): item for item in algorithms}
    judgments: list[Judgment] = []
    for size in SIZES:
        ratios = []
        for _, medians in loaded:
            ikj = medians[(size, "ikj")]
            if ikj <= 0:
                fail(f"{display_path(path)} 的 ikj 中位数不是正数")
            ratios.append(medians[(size, "blocked_64")] / ikj)
        decision = judge_rule_r(ratios)
        ratio_tag = noise_label(sample_cv(ratios))
        blocked_tag = by_key[(size, "blocked_64")].noise_tag
        ikj_tag = by_key[(size, "ikj")].noise_tag
        tag = NOISE_TAG if any((ratio_tag, blocked_tag, ikj_tag)) else ""
        judgments.append(
            Judgment(
                n=size,
                ratios=tuple(ratios),
                decision=decision,
                noise_tag=tag,
            )
        )
    return ConditionSummary(
        name=name,
        path=path,
        kind=kind,
        run_names=tuple(run_name for run_name, _ in loaded),
        algorithms=tuple(algorithms),
        judgments=tuple(judgments),
    )


def summarize_root(root: Path) -> list[ConditionSummary]:
    return [
        summarize_condition(name, path, kind, runs)
        for name, path, kind, runs in discover(root)
    ]


def _ms(value: Decimal) -> str:
    return f"{value.quantize(Decimal('0.000001'), rounding=ROUND_HALF_UP):f}"


def _ratio(value: Decimal) -> str:
    return f"{value.quantize(Decimal('0.0001'), rounding=ROUND_HALF_UP):f}"


def _cv(value: Decimal | None) -> str:
    if value is None:
        return "不适用"
    percent = (value * Decimal(100)).quantize(Decimal("0.01"), rounding=ROUND_HALF_UP)
    return f"{percent}%"


def _spread(value: Decimal | None) -> str:
    if value is None:
        return "不适用"
    return _ratio(value)


def render(root: Path, conditions: list[ConditionSummary]) -> str:
    lines = [
        "只读 day07 的 CSV，计算跨进程中位数、CV、(max-min)/median，并按规则 R 判定。",
        "不写文件，不运行计时程序。脚本不算 P/E 簇，也不比较 C0 和 C1 谁更稳。",
        "每个进程先取 5 条 ms_per_multiply 的中位数；规则 R 再用 10 个进程的 blocked_64/ikj。",
    ]
    if "baseline-fixture" in root.resolve().parts or any(
        "baseline-fixture" in condition.path.resolve().parts for condition in conditions
    ):
        lines.append("格式测试用，非实测。这些数字不能写入正式报告或 metadata。")
    lines.append("")
    for condition in conditions:
        lines.append(f"## 条件 {condition.name}")
        lines.append(f"目录：{display_path(condition.path)}")
        lines.append(f"进程数：{len(condition.run_names)}")
        if condition.kind == "core":
            lines.append("这是核扫描。脚本只列出各核的中位数，P/E 是否分成两簇由零醛看完再写。")
        lines.append("")
        lines.append("| n | 算法 | 中位数 | CV | (max-min)/median | 标签 |")
        lines.append("| ---: | --- | ---: | ---: | ---: | --- |")
        for stats in condition.algorithms:
            lines.append(
                f"| {stats.n} | {stats.algorithm} | {_ms(stats.median)} | "
                f"{_cv(stats.cv)} | {_spread(stats.spread)} | {stats.noise_tag} |"
            )
        lines.append("")
        for judgment in condition.judgments:
            rendered = " ".join(_ratio(ratio) for ratio in judgment.ratios)
            lines.append(f"n={judgment.n} 各进程 blocked_64/ikj：{rendered}")
            median_text = (
                "无"
                if judgment.decision.median_ratio is None
                else _ratio(judgment.decision.median_ratio)
            )
            tag = f" 标签：{judgment.noise_tag}" if judgment.noise_tag else ""
            lines.append(
                f"n={judgment.n} 规则 R：{judgment.decision.verdict}。"
                f"{judgment.decision.reason}。中位比={median_text}。{tag}".rstrip()
            )
        lines.append("")
    return "\n".join(lines).rstrip() + "\n"


def main(argv: list[str] | None = None) -> None:
    if argv is None:
        argv = sys.argv[1:]
    if len(argv) != 1:
        print("用法: python3 scripts/summarize-baseline.py <结果目录>", file=sys.stderr)
        print("没有其它参数。只读 day07 CSV，不写文件，不运行计时程序。", file=sys.stderr)
        raise SystemExit(2)
    root = Path(argv[0])
    try:
        sys.stdout.reconfigure(encoding="utf-8")
    except Exception:
        pass
    conditions = summarize_root(root)
    sys.stdout.write(render(root, conditions))


if __name__ == "__main__":
    main()
