#!/usr/bin/env python3
"""汇编关键字脚本的夹具测试。夹具不是实测。

这是什么：检查 vpmulld 不会被再算进 pmulld，vec-report 能拆行，判断列是空的。
谁写的：测试由 AI 维护。不接入 tests/smoke.sh。
怎么运行：python3 tests/test_scan_asm_keywords.py
限制：失败只说明脚本或夹具不一致。不能把夹具计数写进正式报告。
"""

from __future__ import annotations

import importlib.util
import subprocess
import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT = ROOT / "scripts" / "scan-asm-keywords.py"
FIXTURE = ROOT / "tests" / "fixtures" / "asm-keyword-fixture"


def load_module():
    spec = importlib.util.spec_from_file_location("scan_asm_keywords", SCRIPT)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


class ScanAsmTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mod = load_module()

    def test_fixture_is_labeled(self):
        text = (FIXTURE / "README.md").read_text(encoding="utf-8")
        self.assertIn("格式测试用，非实测", text)

    def test_keyword_counts_do_not_double_count_vpmulld(self):
        functions = self.mod.scan_asm((FIXTURE / "sample.asm").read_text(encoding="utf-8"))
        by_short = {item.short: item.counts for item in functions}
        self.assertEqual(by_short["matmul_ikj"]["vpmulld"], 1)
        self.assertEqual(by_short["matmul_ikj"]["pmulld"], 1)
        self.assertEqual(by_short["matmul_ikj"]["ymm"], 3)
        self.assertEqual(by_short["matmul_ikj"]["xmm"], 2)
        self.assertEqual(by_short["matmul_blocked"]["vpmulld"], 0)
        self.assertEqual(by_short["matmul_blocked"]["pmulld"], 2)
        self.assertEqual(by_short["matmul_blocked"]["xmm"], 4)
        self.assertEqual(by_short["other_helper"]["vpmulld"], 0)
        self.assertNotIn("（函数外）", by_short)

    def test_vec_report_rows_and_blank_judgment(self):
        rows = self.mod.scan_vec((FIXTURE / "sample-vec.txt").read_text(encoding="utf-8"))
        parsed = [(row.line, row.vectorized, row.reason) for row in rows]
        self.assertEqual(
            parsed,
            [
                (81, "是", ""),
                (90, "否", "1200"),
                (110, "否", "1204"),
                (4, "是", ""),
                (5, "否", "501"),
            ],
        )
        rendered = self.mod.render(
            self.mod.scan_asm((FIXTURE / "sample.asm").read_text(encoding="utf-8")),
            rows,
            ["fixture"],
        )
        self.assertIn("| 我的判断 |", rendered)
        self.assertIn("| matmul_ikj | matmul_ikj | 1 | 1 | 3 | 2 |  |", rendered)
        self.assertNotIn("已向量化", rendered)
        self.assertIn("不改源码去迎合编译器", rendered)

    def test_cli_prints_table(self):
        completed = subprocess.run(
            [
                sys.executable,
                str(SCRIPT),
                "--asm",
                str(FIXTURE / "sample.asm"),
                "--vec-report",
                str(FIXTURE / "sample-vec.txt"),
            ],
            check=False,
            capture_output=True,
            text=True,
        )
        self.assertEqual(completed.returncode, 0, completed.stderr)
        self.assertIn("matmul_blocked", completed.stdout)
        self.assertIn("| 90 | 否 | 1200 |", completed.stdout)


if __name__ == "__main__":
    unittest.main()
