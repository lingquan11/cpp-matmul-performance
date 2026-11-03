#!/usr/bin/env python3
"""数据流账本的检查。不接入 tests/smoke.sh。待零醛实现后接入。

它是什么：未实现时 SKIP 数值对照；CLI 未实现必须非零退出，且不打印次数。
谁写的：测试由 AI 维护。count_os / count_ws / count_is 不是在这里实现的。
怎么运行：python3 tests/test_dataflow_ledger.py
限制：下面的总数是 Claude 给出的参考值，待零醛手推核对；WS/IS 的值取决于计数口径。
本测试不把这些数写成程序算出的访问次数。不含 SCALE-Sim。不记时。

计数口径（待零醛确认）：C 的部分和第一趟是否计读？
只计片外/缓冲访问，还是也计 PE 间传递？
"""

from __future__ import annotations

import importlib.util
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "scripts" / "dataflow-ledger.py"

# Claude 给出的参考值，待零醛手推核对；WS/IS 的值取决于计数口径。
# 提示词只给了每种数据流一个总数，没有给出 A/B/C 分项。
# 这个总数是否等于六项读写之和，也属于待核对的口径，不能当成已经证实。
REFERENCE_CASES = (
    {
        "M": 64,
        "K": 64,
        "N": 64,
        "R": 8,
        "C": 8,
        "totals": {"OS": 69632, "WS": 98304, "IS": 98304},
    },
    {
        "M": 64,
        "K": 64,
        "N": 64,
        "R": 16,
        "C": 4,
        "totals": {"OS": 86016, "WS": 98304, "IS": 147456},
    },
)

REFERENCE_NUMBERS = ("69632", "98304", "86016", "147456")


def load_ledger():
    spec = importlib.util.spec_from_file_location("dataflow_ledger", SCRIPT)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"无法加载 {SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def fail(message: str) -> None:
    print(f"FAIL {message}", file=sys.stderr)
    raise SystemExit(1)


def check_framework(ledger) -> None:
    sample = {key: 1 for key in ledger.COUNT_KEYS}
    if ledger.total_accesses(sample) != 6:
        fail("自检：六项都是 1 时总数应为 6。这不是账本结果。")
    if ledger.compare_closed_form(6, 6) != "MATCH":
        fail("自检：相同总数应为 MATCH。")
    if ledger.compare_closed_form(6, 7) != "MISMATCH":
        fail("自检：不同总数应为 MISMATCH。")
    try:
        ledger.total_accesses({"A_reads": 1})
    except ValueError:
        pass
    else:
        fail("自检：缺字段时应拒绝制表。")
    try:
        ledger.total_accesses({key: -1 for key in ledger.COUNT_KEYS})
    except ValueError:
        pass
    else:
        fail("自检：负数应拒绝。")
    print("PASS framework self-check（用 1 和 6，不是访存次数）")


def check_counts(ledger) -> tuple[int, int]:
    skipped = 0
    passed = 0
    functions = {"OS": ledger.count_os, "WS": ledger.count_ws, "IS": ledger.count_is}
    for case in REFERENCE_CASES:
        shape = (case["M"], case["K"], case["N"], case["R"], case["C"])
        for dataflow, fn in functions.items():
            label = (
                f"{dataflow} M={shape[0]} K={shape[1]} N={shape[2]} "
                f"R={shape[3]} C={shape[4]}"
            )
            try:
                counts = fn(*shape)
            except NotImplementedError as exc:
                if str(exc) != "TODO(零醛)":
                    fail(f"{label} 的未实现文本应为 TODO(零醛)，实际 {exc}")
                print(
                    f"SKIP {label} reason=TODO(零醛) "
                    f"Claude参考值={case['totals'][dataflow]} "
                    "待零醛手推核对；WS/IS 取决于计数口径"
                )
                skipped += 1
                continue
            try:
                total = ledger.total_accesses(counts)
            except ValueError as exc:
                fail(f"{label} 已实现但结果不能加总：{exc}")
            expected = case["totals"][dataflow]
            if total != expected:
                fail(
                    f"{label} 总数 {total} 与待核对参考值 {expected} 不同。"
                    "参考值仍待零醛手推核对。"
                )
            print(f"PASS {label} total={total}（对照了待核对参考值）")
            passed += 1
    return passed, skipped


def run_cli(args: list[str]) -> subprocess.CompletedProcess[str]:
    return subprocess.run(
        [sys.executable, str(SCRIPT), *args],
        check=False,
        capture_output=True,
        text=True,
    )


def check_cli() -> None:
    missing = run_cli([])
    if missing.returncode != 2:
        fail(f"缺少参数时应退出 2，实际 {missing.returncode}")
    help_text = run_cli(["--help"])
    if help_text.returncode != 0:
        fail(f"--help 应退出 0，实际 {help_text.returncode}")
    if "待零醛确认" not in help_text.stdout:
        fail("帮助文本应保留计数口径「待零醛确认」")
    bad_flow = run_cli(["--M", "64", "--K", "64", "--N", "64", "--R", "8", "--C", "8", "--dataflow", "os"])
    if bad_flow.returncode != 2:
        fail(f"小写 dataflow 应退出 2，实际 {bad_flow.returncode}")

    for dataflow in ("OS", "WS", "IS"):
        proc = run_cli(
            [
                "--M",
                "64",
                "--K",
                "64",
                "--N",
                "64",
                "--R",
                "8",
                "--C",
                "8",
                "--dataflow",
                dataflow,
                "--closed-form-total",
                "1",
            ]
        )
        combined = proc.stdout + proc.stderr
        if proc.returncode == 0:
            fail(f"{dataflow} 未实现时不应退出 0")
        if "未实现" not in proc.stderr:
            fail(f"{dataflow} 未实现时应在 stderr 写「未实现」：{proc.stderr}")
        if "TODO(零醛)" not in proc.stderr:
            fail(f"{dataflow} 的 stderr 应含 TODO(零醛)")
        for number in REFERENCE_NUMBERS:
            if number in combined:
                fail(f"{dataflow} 的输出含参考值 {number}，未实现时不能打印次数")
        if "MATCH" in proc.stdout or proc.stdout.strip():
            fail(f"{dataflow} 未实现时 stdout 应为空，实际 {proc.stdout!r}")
    print("PASS cli：未实现时非零退出，stdout 无访问次数")


def main() -> None:
    ledger = load_ledger()
    check_framework(ledger)
    check_cli()
    passed, skipped = check_counts(ledger)
    print(
        f"dataflow ledger tests: PASS {passed} implemented comparisons; "
        f"SKIP {skipped} unimplemented. No timings written."
    )


if __name__ == "__main__":
    main()
