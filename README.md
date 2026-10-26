# cpp-matmul-performance

用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 重要。

我是东南大学电子信息专业大一。

## 当前做到哪

i-j-k 小规模正确性（Day 2）已有。`src/day02_matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。

Day 4 本地 64、128、256 的截图转录在 [`experiments/results/2026-10-26_day04-local/`](experiments/results/2026-10-26_day04-local/README.md)。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](src/day04_matmul.cpp)。那次实际跑的完整基准源码和原始 CSV 还没上传。

Day 5 的循环顺序（`ijk` 与 `ikj`）已验收。今天 30 条和昨天补上的 15 条分开放，在 [`experiments/results/2026-10-26_day05-loop-order/`](experiments/results/2026-10-26_day05-loop-order/README.md)。我提交的双版本在 [`src/day05_ijk_ikj_benchmark.cpp`](src/day05_ijk_ikj_benchmark.cpp)。[`src/gpt-templates/`](src/gpt-templates/README.md) 里的是 GPT 模板，不是我独立写的。版本快照在 [`code_history/`](code_history/README.md)。

下一步我看缓存行和空间局部性，再以现在的 `ijk`、`ikj` 为基线做分块。Day 4 的编译器和原始 CSV 仍未补。CPU 型号、编译器具体版本、Day 5 学习用时也未记录。

眼下的安排以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准。

## 怎么跑

仓库里现在没有可跑的云端基准脚本。学习程序在 [`src/`](src/README.md)。

## 结果在哪

仓里现在有的是 Day 4 本地 64、128、256 的截图转录，在 [`experiments/results/2026-10-26_day04-local/`](experiments/results/2026-10-26_day04-local/README.md)，短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 的两份 CSV 在 [`experiments/results/2026-10-26_day05-loop-order/`](experiments/results/2026-10-26_day05-loop-order/README.md)，短记录在 [Day 5 循环顺序](experiments/reports/2026-10-26-day05-loop-order.md)。

报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。大规模扫描结果我还没放进仓。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 5 在 [`notes/day05-notes.md`](notes/day05-notes.md)
- 每日归档 `dayNN-record.md`，Day 5 在 [`notes/day05-record.md`](notes/day05-record.md)
- 每天的流水 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 学习程序（Day 1 单元素，Day 2 的 2×2，Day 4 单次计时，Day 5 双版本）。[`src/gpt-templates/`](src/gpt-templates/README.md) 只放 GPT 模板 |
| [`code_history/`](code_history/README.md) | Day 4（V01–V06）和 Day 5 的版本快照 |
| [`benchmarks/`](benchmarks/README.md) | 占位。以后放我自己测稳后再公开的计时驱动。现在没有可跑的云端基准脚本 |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/` |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录和阶段报告 |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。Day 4 是截图转录；Day 5 是两份原样 CSV |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |
