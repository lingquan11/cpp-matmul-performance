# 2026-11 归档的脚手架

2026-11-22 按收尾战役 T01 归档。这些都是 AI 搭的脚手架或计划，kernel 没有实现，也没有实测数据。归档是因为收尾战役（2026-11-23 → 12-06）不做它们，**不是因为它们没有价值**。目录结构与原路径一致，取回时 `git mv` 回原处即可。

| 主题 | 内容 | 去向 |
|---|---|---|
| 段一任务卡 | `plan/seg1-scale-sweep.md`（17 张卡，7742 行）、`plan/README.md` | 被 `tasks/T02`、`T03` 取代 |
| 行跨度 LDA | `templates/gpt/layout-lda.cpp`、`scripts/run-layout-lda.bat`、`scripts/summarize-layout-lda.py`、`tests/layout-lda-correctness.cpp`、`tests/test_summarize_layout_lda.py`、`tests/fixtures/layout-lda-fixture/`、报告与结果模板 | IDEAS.md |
| 六种循环顺序（1002-I4） | `templates/gpt/exp-loop-orders.cpp`、`templates/gpt/loop-orders-run-metadata.md`、`scripts/run-loop-orders-c1.ps1`、`tests/loop-orders-correctness.cpp` | IDEAS.md |
| 数据流账本（OS/WS/IS） | `scripts/dataflow-ledger.py`、`tests/test_dataflow_ledger.py`、报告模板、`notes/dayNN-dataflow-derivation.md` | 寒假 IC 战役做 NPU 数据流时取回 |
| 打包分块 | `templates/gpt/exp-packed-block.cpp`、`scripts/run-packed-block.bat`、`scripts/summarize-packed-block.py`、`tests/packed-block-correctness.cpp`、`tests/fixtures/packed-block/`、报告模板 | 做 CAMPAIGN 加分线 B03 时取回 |
| GEMM 形状 | `templates/gpt/exp-gemm-shapes.cpp`、`scripts/run-gemm-shapes.bat`、`scripts/summarize-gemm-shapes.py`、`scripts/check-shape-overflow.py`、`tests/gemm-shapes-correctness.cpp`、`tests/test-gemm-summary.py`、`tests/test-shape-overflow.py`、报告模板、`notes/gemm-shape-mapping.md` | IDEAS.md |
| PMC / Plan B | `templates/gpt/pmc-probe.cpp`、`scripts/run-planb.bat`、`scripts/summarize-planb.py`、`tests/test-planb-summary.py`、报告与结果模板、`notes/pmc-feasibility.md` | 做加分线 B01（VTune）时参考 |
| 复用模型 | 报告模板、`scripts/run-block-sweep.bat` | 已并入 T02；`scripts/block-size-table.py` 留在原处 |
| 空笔记模板 | `notes/dayNN-asm-reading.md`、`notes/dayNN-hw-facts.md` | 被 `notes/block-size-hw-facts.md` 和 T04 报告取代 |

不在本目录、但同时移动的：`PROJECT_STATE.md` 和两份 `handoff-*.md` 移到了 `notes/history/`。

这些文件不被 `tests/smoke.sh` 和 CI 编译或引用。
