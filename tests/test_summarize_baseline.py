#!/usr/bin/env python3
"""规则 R 的格式测试。夹具数字不是实测。

这是什么：检查 summarize-baseline.py 的三种判定，以及测量模板里没有编造的计时。
谁写的：测试由 AI 维护。不代替零醛的实测，也不接入 tests/smoke.sh。
怎么运行：在仓库根目录执行 python3 tests/test_summarize_baseline.py
限制：失败只说明脚本或夹具不一致。不能把夹具数字写进正式报告。
"""

from __future__ import annotations

import csv
import importlib.util
import subprocess
import sys
import tempfile
import unittest
from decimal import Decimal
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
FIXTURE = ROOT / "tests" / "fixtures" / "baseline-fixture"
SCRIPT = ROOT / "scripts" / "summarize-baseline.py"
COLUMNS = [
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
]
SIZES = ("64", "128", "256")
ALGORITHMS = ("ijk", "ikj", "blocked_16", "blocked_32", "blocked_64")


def load_module():
    spec = importlib.util.spec_from_file_location("summarize_baseline", SCRIPT)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def write_run(directory: Path, value_for) -> None:
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / "matmul_benchmark_day7_blocking.csv"
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(COLUMNS)
        for size in SIZES:
            for algorithm in ALGORITHMS:
                for sample in range(1, 6):
                    ms = value_for(size, algorithm, sample)
                    writer.writerow(
                        [size, algorithm, ms, sample, 1, ms, ms, 0, 0, 0, "YES", "PASS"]
                    )


class BaselineRuleTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mod = load_module()

    def test_fixture_is_labeled_and_covers_three_rule_cases(self):
        readme = (FIXTURE / "README.md").read_text(encoding="utf-8")
        self.assertIn("格式测试用，非实测", readme)
        conditions = {
            item.name: item
            for item in self.mod.summarize_root(FIXTURE)
        }
        self.assertEqual(
            set(conditions),
            {"same-side-under-5pct", "nine-of-ten", "satisfies-r"},
        )

        under = conditions["same-side-under-5pct"]
        self.assertEqual(under.run_names, tuple(f"run{index:02d}" for index in range(1, 11)))
        for judgment in under.judgments:
            self.assertEqual(judgment.decision.verdict, "不判")
            self.assertIn("不足 5%", judgment.decision.reason)
            self.assertEqual(judgment.decision.median_ratio, Decimal("1.04"))
            self.assertEqual(judgment.noise_tag, "")
        blocked = next(
            item for item in under.algorithms if item.n == "64" and item.algorithm == "blocked_64"
        )
        self.assertEqual(blocked.spread, Decimal(0))
        self.assertEqual(blocked.noise_tag, "")

        nine = conditions["nine-of-ten"]
        for judgment in nine.judgments:
            self.assertEqual(judgment.decision.verdict, "不判")
            self.assertIn("9/10", judgment.decision.reason)
            self.assertEqual(judgment.decision.median_ratio, Decimal("1.2"))
            self.assertNotEqual(judgment.decision.verdict, "慢")

        yes = conditions["satisfies-r"]
        by_n = {item.n: item for item in yes.judgments}
        self.assertEqual(by_n["64"].decision.verdict, "慢")
        self.assertEqual(by_n["64"].decision.median_ratio, Decimal("1.1"))
        self.assertEqual(by_n["64"].noise_tag, "环境噪声偏高")
        self.assertIn("至少 5%", by_n["64"].decision.reason)
        noisy = next(
            item for item in yes.algorithms if item.n == "64" and item.algorithm == "blocked_64"
        )
        self.assertEqual(noisy.process_medians, (Decimal("110"),) * 9 + (Decimal("140"),))
        self.assertGreaterEqual(noisy.cv, Decimal("0.03"))
        self.assertEqual(noisy.noise_tag, "环境噪声偏高")
        self.assertEqual(by_n["128"].decision.verdict, "快")
        self.assertEqual(by_n["128"].decision.median_ratio, Decimal("0.9"))
        self.assertEqual(by_n["128"].noise_tag, "")
        self.assertEqual(by_n["256"].decision.verdict, "慢")
        self.assertEqual(by_n["256"].decision.median_ratio, Decimal("1.05"))
        self.assertEqual(by_n["256"].noise_tag, "")

    def test_cli_prints_fixture_warning_and_verdicts(self):
        result = subprocess.run(
            [sys.executable, str(SCRIPT), str(FIXTURE)],
            check=False,
            capture_output=True,
            text=True,
        )
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("格式测试用，非实测", result.stdout)
        self.assertIn("规则 R：不判", result.stdout)
        self.assertIn("规则 R：慢", result.stdout)
        self.assertIn("规则 R：快", result.stdout)
        self.assertIn("环境噪声偏高", result.stdout)
        self.assertNotIn("待测", result.stdout)

    def test_rule_boundaries_and_sample_median(self):
        judge = self.mod.judge_rule_r
        self.assertEqual(judge([Decimal("1.05")] * 10).verdict, "慢")
        self.assertEqual(judge([Decimal("0.95")] * 10).verdict, "快")
        self.assertEqual(judge([Decimal("1.049")] * 10).verdict, "不判")
        self.assertIn("不足 5%", judge([Decimal("1.049")] * 10).reason)
        self.assertEqual(judge([Decimal("1")] * 10).verdict, "不判")
        self.assertEqual(judge([Decimal("1.2")] * 9).verdict, "不判")
        self.assertIn("不是 10", judge([Decimal("1.2")] * 9).reason)
        self.assertEqual(
            self.mod.median([Decimal(item) for item in ("1", "100", "10", "100", "1")]),
            Decimal(10),
        )
        self.assertEqual(
            self.mod.median([Decimal(item) for item in ("1", "2", "3", "4")]),
            Decimal("2.5"),
        )
        self.assertEqual(self.mod.noise_label(Decimal("0.03")), "环境噪声偏高")
        self.assertEqual(self.mod.noise_label(Decimal("0.029999")), "")
        self.assertEqual(self.mod.noise_label(None), "")

    def test_process_median_uses_middle_sample_not_mean(self):
        def value_for(size, algorithm, sample):
            if algorithm == "blocked_64":
                return ("3", "1", "2", "100", "0.5")[sample - 1]
            return "1"

        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            for index in range(1, 11):
                write_run(root / f"run{index:02d}", value_for)
            summary = self.mod.summarize_root(root)[0]
            blocked = next(
                item
                for item in summary.algorithms
                if item.n == "128" and item.algorithm == "blocked_64"
            )
            self.assertEqual(blocked.process_medians, (Decimal(2),) * 10)
            judgment = next(item for item in summary.judgments if item.n == "128")
            self.assertEqual(judgment.decision.verdict, "慢")
            self.assertEqual(judgment.decision.median_ratio, Decimal(2))

    def test_bad_shape_and_empty_template_do_not_invent_numbers(self):
        with self.assertRaises(SystemExit) as missing:
            self.mod.main([])
        self.assertEqual(missing.exception.code, 2)

        template = ROOT / "experiments" / "results" / "YYYY-MM-DD-measure-baseline-windows"
        with self.assertRaises(SystemExit) as empty:
            self.mod.summarize_root(template)
        self.assertEqual(empty.exception.code, 1)
        metadata = (template / "metadata.md").read_text(encoding="utf-8")
        report = (ROOT / "experiments" / "reports" / "YYYY-MM-DD-measure-baseline.md").read_text(
            encoding="utf-8"
        )
        rules = (ROOT / "experiments" / "reports" / "measurement-rules.md").read_text(
            encoding="utf-8"
        )
        self.assertIn("待测", metadata)
        self.assertIn("待测", report)
        self.assertIn("待零醛实测后填写", report)
        self.assertNotIn("1.04", metadata)
        self.assertNotIn("1.04", report)
        self.assertLessEqual(len(rules.splitlines()), 10)
        self.assertIn("草案，待零醛实测后确认", rules)
        self.assertIn("建议以 R 取代，待零醛确认", rules)
        self.assertIn("环境噪声偏高", rules)
        self.assertIn("1.5", rules)
        for name in ("core-scan", "c0", "c1"):
            self.assertTrue((template / name / ".gitkeep").is_file())
            self.assertEqual(list((template / name).rglob("*.csv")), [])

        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary) / "partial"
            write_run(root / "run01", lambda size, algorithm, sample: "1")
            (root / "run02").mkdir(parents=True)
            with self.assertRaises(SystemExit) as partial:
                self.mod.summarize_root(root)
            self.assertEqual(partial.exception.code, 1)

            bad = Path(temporary) / "bad"
            write_run(bad / "run01", lambda size, algorithm, sample: "1")
            path = bad / "run01" / "matmul_benchmark_day7_blocking.csv"
            text = path.read_text(encoding="utf-8")
            path.write_text(text.replace("ms_per_multiply", "elapsed_ms"), encoding="utf-8")
            with self.assertRaises(SystemExit) as header:
                self.mod.summarize_root(bad)
            self.assertEqual(header.exception.code, 1)

    def test_archived_median_script_has_no_csv_flag(self):
        source = (ROOT / "experiments" / "summarize_medians.py").read_text(encoding="utf-8")
        self.assertNotIn("--csv", source)

    def test_batch_scripts_keep_runs_in_their_own_directories(self):
        core = (ROOT / "scripts" / "run-core-scan.bat").read_bytes()
        pair = (ROOT / "scripts" / "run-c0-c1.bat").read_bytes()
        self.assertIn(b"\r\n", core)
        self.assertIn(b"\r\n", pair)
        core_text = core.decode("utf-8")
        pair_text = pair.decode("utf-8")
        self.assertIn('start "" /wait /high /affinity', core_text)
        self.assertIn("1.5 小时熔断", core_text)
        self.assertIn(r"core-scan\core", core_text)
        self.assertIn("dry-run", core_text)
        self.assertIn("不带 0x", core_text)
        self.assertIn("matmul_benchmark_day7_blocking.csv", core_text)
        self.assertIn('set "MASK=1"', core_text)
        self.assertIn('set "MASK=80000"', core_text)
        self.assertIn('set "P_CORE_MASK=TODO"', pair_text)
        self.assertIn(r"c0\run", pair_text)
        self.assertIn(r"c1\run", pair_text)
        self.assertIn("5 小时", pair_text)
        self.assertIn('start "" /wait /high /affinity', pair_text)
        self.assertIn('start "" /wait "%EXE%"', pair_text)
        self.assertIn("不带 0x", pair_text)
        self.assertNotIn('set "P_CORE_MASK=1"', pair_text)


if __name__ == "__main__":
    unittest.main()
