#!/usr/bin/env python3
"""行跨度阈值的格式测试。夹具数字不是实测。

这是什么：检查「行跨度显著影响 / 无影响 / 灰区 / 不判」，以及 ikj 的 3% 用词。
谁写的：测试由 AI 维护。不接入 tests/smoke.sh。
怎么运行：python3 tests/test_summarize_layout_lda.py
限制：临时 CSV 里的毫秒是故意取的整数，不能写进正式报告。
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
SCRIPT = ROOT / "scripts" / "summarize-layout-lda.py"
COLUMNS = [
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
]


def load_module():
    spec = importlib.util.spec_from_file_location("summarize_layout_lda", SCRIPT)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def write_cell(root: Path, n: int, lda: int, algo: str, run: int, samples: list[str]) -> None:
    directory = root / f"n{n}" / f"lda-{lda}" / algo / f"run{run:02d}"
    directory.mkdir(parents=True, exist_ok=True)
    path = directory / "layout-lda.csv"
    with path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle, lineterminator="\n")
        writer.writerow(COLUMNS)
        for index, ms in enumerate(samples, start=1):
            writer.writerow(
                [n, lda, algo, ms, index, 1, ms, ms, 0, 0, 0, "YES", "PASS"]
            )


class LayoutThresholdTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mod = load_module()

    def test_readme_says_fixture(self):
        text = (ROOT / "tests" / "fixtures" / "layout-lda-fixture" / "README.md").read_text(
            encoding="utf-8"
        )
        self.assertIn("格式测试用，非实测", text)

    def test_threshold_words(self):
        judge = self.mod.judge_row_stride
        below = [Decimal("0.80")] * 10
        mixed = [Decimal("0.80")] * 9 + [Decimal("1.10")]
        self.assertEqual(judge(Decimal("0.80"), below), "行跨度显著影响")
        self.assertEqual(judge(Decimal("0.90"), below), "行跨度显著影响")
        self.assertEqual(judge(Decimal("0.80"), mixed), "灰区")
        self.assertEqual(judge(Decimal("0.95"), below), "灰区")
        self.assertEqual(judge(Decimal("0.97"), mixed), "无影响")
        self.assertEqual(judge(Decimal("1.00"), mixed), "无影响")
        self.assertEqual(judge(Decimal("1.03"), below), "无影响")
        self.assertEqual(judge(Decimal("1.04"), below), "灰区")
        self.assertEqual(judge(Decimal("0.80"), below[:9]), "不判")

    def test_ikj_span_inclusive_three_percent(self):
        self.assertEqual(self.mod.ikj_label(Decimal("0.03")), "<=3%")
        self.assertEqual(self.mod.ikj_label(Decimal("0.0300001")), "超过 3%")
        self.assertEqual(self.mod.ikj_label(None), "LDA 水平不足，不比较")

    def test_tree_significant_and_stable_ikj(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for run in range(1, 11):
                write_cell(root, 256, 256, "ijk", run, ["10", "10", "10"])
                write_cell(root, 256, 272, "ijk", run, ["8", "8", "8"])
                write_cell(root, 256, 256, "ikj", run, ["5", "5", "5"])
                write_cell(root, 256, 272, "ikj", run, ["5", "5", "5"])
            text = self.mod.render(root, self.mod.load_tree(root))
        self.assertIn("判定 = 行跨度显著影响", text)
        self.assertIn("0.8", text)
        self.assertIn("变化=<=3%", text)
        self.assertIn("ideas/matmul-1002-i1-measurement-baseline", text)

    def test_tree_ikj_over_three_percent(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for run in range(1, 11):
                write_cell(root, 256, 256, "ijk", run, ["10"])
                write_cell(root, 256, 272, "ijk", run, ["10"])
                write_cell(root, 256, 256, "ikj", run, ["100"])
                write_cell(root, 256, 272, "ikj", run, ["104"])
            text = self.mod.render(root, self.mod.load_tree(root))
        self.assertIn("判定 = 无影响", text)
        self.assertIn("变化=超过 3%", text)

    def test_cli_on_temp_tree(self):
        with tempfile.TemporaryDirectory() as tmp:
            root = Path(tmp)
            for run in range(1, 11):
                write_cell(root, 256, 256, "ijk", run, ["9", "10", "11"])
                write_cell(root, 256, 272, "ijk", run, ["9", "10", "11"])
            completed = subprocess.run(
                [sys.executable, str(SCRIPT), str(root)],
                check=False,
                capture_output=True,
                text=True,
            )
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertIn("判定 = 无影响", completed.stdout)
        self.assertIn("没有 ikj 行", completed.stdout)


if __name__ == "__main__":
    unittest.main()
