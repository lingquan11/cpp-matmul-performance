# cpp-matmul-performance

用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 重要。

我是东南大学电子信息专业大一。

## 当前做到哪

i-j-k 小规模正确性（Day 2）已有。`src/day02-matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。

Day 4 是 2026-10-26。同一目录里有截图转录和程序导出两份 15 条，数值不同，都不覆盖：[`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)。我粘贴过的单次计时程序在 [`src/day04-matmul.cpp`](src/day04-matmul.cpp)。截图转录那组的批量源码还没上传。

Day 5 的循环顺序（`ijk` 与 `ikj`）已验收通过，只计今天的 30 条：[`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)。我提交的双版本在 [`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp)。[`templates/gpt/`](templates/gpt/README.md) 里的是 GPT 模板，不是我独立写的。版本快照在 [`code_history/`](code_history/README.md)。

下一步我看缓存行和空间局部性，再以现在的 `ijk`、`ikj` 为基线做分块。Day 4 截图转录的批量源码仍未补。比 Visual Studio 2026 18.x 更细的编译器版本号，以及 Day 5 学习用时，也未记录。

眼下的安排以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准。

## 怎么跑

`benchmarks/` 只是占位。正式计时驱动等我本地测稳后再放，仓库里没有云端可跑的基准。[`.github/workflows/compile.yml`](.github/workflows/compile.yml) 调用 [`tests/smoke.sh`](tests/smoke.sh)：用 `g++ -std=c++17 -O2` 编译 [`src/`](src/README.md) 下现存的 `.cpp`，并检查 Day 1 输出 `11`、Day 2 输出 `11 10` 和 `19 30`。这不是完整测试，也不是性能测试。Day 4、Day 5 只编译，不运行。计时复现：未验证。Day 5 双版本源码里的泛型 lambda 需要 C++14 或更新标准；我实际用的编译器版本未记录。

## 结果在哪

仓里现在有的是 Day 4 的截图转录和程序导出，在 [`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)，短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 的 30 条在 [`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)，短记录在 [Day 5 循环顺序](experiments/reports/2026-10-26-day05-loop-order.md)。

报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。虚拟机或大规模扫描的结果不在库里。材料缺口：待用户提供。材料已按职责归档进 src / notes / experiments，验收 ZIP 原件不再入库。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 5 在 [`notes/day05-notes.md`](notes/day05-notes.md)
- 每日归档 `dayNN-record.md`，Day 5 在 [`notes/day05-record.md`](notes/day05-record.md)
- 每天的索引 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 这一阶段的现行成品（Day 1 单元素，Day 2 的 2×2，Day 4 单次计时，Day 5 双版本）。文件名用连字符 |
| [`templates/gpt/`](templates/gpt/README.md) | GPT 模板。不是我独立写的，也不放在 `src/` |
| [`code_history/`](code_history/README.md) | 各日代码演变快照。和 `src/` 并存，哈希不同不合并。快照文件名保持原样 |
| [`benchmarks/`](benchmarks/README.md) | 占位。正式驱动等我本地测稳后再放。没有云端可跑基准 |
| [`tests/`](tests/README.md) | 最小冒烟：编译 `src/`，检查 Day 1、Day 2 的已知输出。不含性能测试 |
| [`.github/workflows/compile.yml`](.github/workflows/compile.yml) | 只做上面的编译检查。不是计时 CI |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/` |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录和阶段报告 |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。Day 4 的转录和程序导出分文件放在同一目录；Day 5 是 30 条程序导出 |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |
