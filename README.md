# cpp-matmul-performance

用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 重要。

我是东南大学电子信息专业大一。

## 当前做到哪

i-j-k 小规模正确性（Day 2）已有。`src/day02_matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。

Day 4 本地 64、128、256 的截图转录在 [`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](src/day04_matmul.cpp)。那次实际跑的完整基准源码和电脑直接导出的原始 CSV 还没上传。

Day 5 的循环顺序（`ijk` 与 `ikj`）已验收通过。今天的 30 条程序导出在 [`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)。前一轮只测 `ikj` 的 15 条另放在 [`experiments/results/2026-10-25-day05-ikj-only-prior/`](experiments/results/2026-10-25-day05-ikj-only-prior/README.md)，不是 Day 4 的 ijk 基线。我提交的双版本在 [`src/day05_ijk_ikj_benchmark.cpp`](src/day05_ijk_ikj_benchmark.cpp)。[`templates/gpt/`](templates/gpt/README.md) 里的是 GPT 模板，不是我独立写的。版本快照在 [`code_history/`](code_history/README.md)。

下一步我看缓存行和空间局部性，再以现在的 `ijk`、`ikj` 为基线做分块。Day 4 的编译器和原始 CSV 仍未补。CPU 型号、编译器具体版本、Day 5 学习用时也未记录。

眼下的安排以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准。

## 怎么跑

仓库里现在没有可跑的云端基准脚本，也没有 `.github/workflows`。`benchmarks/` 只是占位。学习程序在 [`src/`](src/README.md)。Day 1、Day 2 可以用 g++ 编译运行。Day 5 双版本源码里的泛型 lambda 需要 C++14 或更新标准；我实际用的编译器版本未记录。

## 结果在哪

仓里现在有的是 Day 4 本地 64、128、256 的截图转录，在 [`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)，短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 今天的程序导出在 [`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)，前一轮单算法在 [`experiments/results/2026-10-25-day05-ikj-only-prior/`](experiments/results/2026-10-25-day05-ikj-only-prior/README.md)，短记录在 [Day 5 循环顺序](experiments/reports/2026-10-26-day05-loop-order.md)。

报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。大规模扫描结果我还没放进仓。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 5 在 [`notes/day05-notes.md`](notes/day05-notes.md)
- 每日归档 `dayNN-record.md`，Day 5 在 [`notes/day05-record.md`](notes/day05-record.md)
- 每天的索引 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 我提交或粘贴过、现在接着看的程序（Day 1 单元素，Day 2 的 2×2，Day 4 单次计时，Day 5 双版本） |
| [`templates/gpt/`](templates/gpt/README.md) | GPT 模板。不是我独立写的，也不放在 `src/` |
| [`code_history/`](code_history/README.md) | Day 4（V01–V06）和 Day 5 的版本快照。前一轮 `ikj` 只在这里 |
| [`benchmarks/`](benchmarks/README.md) | 占位。以后放我自己测稳后再公开的计时驱动。现在没有可跑的云端基准脚本 |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/` |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录和阶段报告 |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。一组一个目录。Day 4 是截图转录；Day 5 今天是程序导出，前一轮另开目录 |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`archive/2026-10-26-day05-final-acceptance/`](archive/2026-10-26-day05-final-acceptance/README.md) | Day 5 最终验收 ZIP 原件。拆开的文件在各目录，原包留着核对 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |
