# cpp-matmul-performance

用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 重要。

我是东南大学电子信息专业大一。

## 路线图

- [x] **阶段 A · 算法实现**：完整三重循环实现矩阵乘法，用小矩阵手算结果核对正确性，并分析时间复杂度。已完成（Day 1–2；Day 3 零矩阵等）。
- [ ] **阶段 B · 性能测量**：计时方法，以及矩阵规模、循环顺序、编译优化等控制变量。进行中。本地计时和循环顺序对比已阶段性完成（Day 4 规模、计时、截图转录与程序导出；Day 5 `ijk` / `ikj` 30 条验收）。编译优化选项还没有单独对比。计时复现未验证。
- [ ] **阶段 C · 分块优化与实验报告**：分块（tiling），并写出可复现的实验报告。已启动（Day 6 概念；Day 7 分块 75 条与边界测试 PASS）。正式报告进行中。分块尚未普遍快过 `ikj`。
- [ ] **阶段 D · 脉动阵列与 SCALE-Sim**：理解脉动阵列数据流，用 SCALE-Sim 对照前面的实测结果。未开始。

## 阶段成果

下表按 [`PROJECT_STATE.md`](PROJECT_STATE.md) 和仓库里现有的报告、结果来写。和状态文件冲突时，以状态文件为准。

| 阶段 | 状态 | 成果 |
| --- | --- | --- |
| A · 算法实现 | 已完成（Day 1–2；Day 3 零矩阵等） | [`src/day01-matrix.cpp`](src/day01-matrix.cpp) 算单个输出元素；[`src/day02-matrix.cpp`](src/day02-matrix.cpp) 是固定 2×2。报告在 [阶段 A](experiments/reports/stage-a-algorithm-implementation.md)。 |
| B · 性能测量 | 进行中；本地计时与循环顺序对比阶段性完成 | Day 4 截图转录与程序导出：[结果](experiments/results/2026-10-26-day04-local/README.md)、[记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 的 30 条：[结果](experiments/results/2026-10-26-day05-loop-order-local/README.md)、[记录](experiments/reports/2026-10-26-day05-loop-order.md)。截图转录那组的完整 benchmark 源码仍缺。计时复现：未验证。 |
| C · 分块优化与实验报告 | 已启动；正式可复现实验报告进行中 | Day 6 概念在 [`notes/day06-notes.md`](notes/day06-notes.md)，没有新测量。Day 7 正式 CSV 75 条全部 PASS，`n=70`、`block_size=32` 边界测试通过：[结果](experiments/results/2026-10-27-day07-blocking-local/README.md)、[分块记录](experiments/reports/2026-10-27-day07-blocking.md)。[`src/day07-matrix-blocking.cpp`](src/day07-matrix-blocking.cpp) 是 GPT 整合版，不是用户本机逐字源码。分块尚未普遍快过 `ikj`。 |
| D · 脉动阵列与 SCALE-Sim | 未开始 | — |

## 当前做到哪

i-j-k 小规模正确性（Day 2）已有。`src/day02-matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。

Day 4 是 2026-10-26。同一目录里有截图转录和程序导出两份 15 条，数值不同，都不覆盖：[`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)。我粘贴过的单次计时程序在 [`src/day04-matmul.cpp`](src/day04-matmul.cpp)。截图转录那组的实际完整 benchmark 源码仍缺；[`code_history/day04/V06_gpt_benchmark_template.cpp`](code_history/day04/V06_gpt_benchmark_template.cpp) 是 GPT 模板，不能冒充执行稿。128 阶偏高样本的分析在 [`notes/supplements/day04-day05-n128-high-samples.md`](notes/supplements/day04-day05-n128-high-samples.md)，CSV 未改。

Day 5 的循环顺序（`ijk` 与 `ikj`）已验收通过，只计 2026-10-26 的 30 条：[`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)。我提交的双版本在 [`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp)。[`templates/gpt/`](templates/gpt/README.md) 里的是 GPT 模板，不是我独立写的。版本快照在 [`code_history/`](code_history/README.md)。

Day 6 是 2026-10-27。概念验收已通过：内存布局、缓存行、空间局部性，以及循环重排和分块的区别。Day 6 没有新代码，也没有新的测量。笔记在 [`notes/day06-notes.md`](notes/day06-notes.md)。

同一天 Day 7 分块矩阵乘法学习验收已通过：正式 CSV 75 条全部 PASS；`n=70`、`block_size=32` 边界测试通过；学习用时 60 分钟。数据在 [`experiments/results/2026-10-27-day07-blocking-local/`](experiments/results/2026-10-27-day07-blocking-local/)，分析在 [`experiments/reports/2026-10-27-day07-blocking.md`](experiments/reports/2026-10-27-day07-blocking.md)。用户中间版在 [`code_history/day07/`](code_history/day07/README.md)。[`src/day07-matrix-blocking.cpp`](src/day07-matrix-blocking.cpp) 是根据学习对话整理的 GPT 整合版，不是用户本机逐字源码；未在用户本机 Visual Studio 重新构建，也不覆盖正式 CSV。GPT 片段在 [`templates/gpt/day07-blocking-snippets.md`](templates/gpt/day07-blocking-snippets.md)。

Day 5 学习用时已补录为 60 分钟。Day 4–Day 7 同机同硬件：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。

下一步可选补边界组合，比较中位数和波动，并学数据复用与分块大小选择。Day 5 的 `ijk`、`ikj` 仍作基线。仍缺的是 Day 4 截图转录那组的实际批量源码，以及 MSVC toolset 独立版本号。补缴说明在 [`notes/supplements/`](notes/supplements/README.md)。

眼下的安排以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准。

## 怎么跑

`benchmarks/` 只是占位。正式计时驱动等我本地测稳后再放，仓库里没有云端可跑的基准。[`.github/workflows/compile.yml`](.github/workflows/compile.yml) 调用 [`tests/smoke.sh`](tests/smoke.sh)：用 `g++ -std=c++17 -O2` 编译 [`src/`](src/README.md) 下现存的 `.cpp`，并检查 Day 1 输出 `11`、Day 2 输出 `11 10` 和 `19 30`。这不是完整测试，也不是性能测试。Day 4、Day 5、Day 7 只编译，不运行。计时复现：未验证。Day 5 双版本和 Day 7 GPT 整合版里的泛型 lambda 需要 C++14 或更新标准。MSVC toolset 独立版本号未记录。

## 结果在哪

仓里现在有的是 Day 4 的截图转录和程序导出，在 [`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)，短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 的 30 条在 [`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)，短记录在 [Day 5 循环顺序](experiments/reports/2026-10-26-day05-loop-order.md)。Day 7 的 75 条在 [`experiments/results/2026-10-27-day07-blocking-local/`](experiments/results/2026-10-27-day07-blocking-local/README.md)，短分析在 [Day 7 分块](experiments/reports/2026-10-27-day07-blocking.md)。

报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。虚拟机或大规模扫描的结果不在库里。材料缺口：待用户提供。材料已按职责归档进 src / notes / experiments，验收 ZIP 原件不再入库。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 5 在 [`notes/day05-notes.md`](notes/day05-notes.md)
- 每日归档 `dayNN-record.md`，Day 5 在 [`notes/day05-record.md`](notes/day05-record.md)
- 每天的索引 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 这一阶段的现行文件。Day 7 的 `day07-matrix-blocking.cpp` 是 GPT 整合版，不是用户逐字源码。文件名用连字符 |
| [`templates/gpt/`](templates/gpt/README.md) | GPT 模板。不是我独立写的，也不放在 `src/` |
| [`code_history/`](code_history/README.md) | 各日代码演变快照。和 `src/` 并存，哈希不同不合并。快照文件名保持原样 |
| [`benchmarks/`](benchmarks/README.md) | 占位。正式驱动等我本地测稳后再放。没有云端可跑基准 |
| [`tests/`](tests/README.md) | 最小冒烟：编译 `src/`，检查 Day 1、Day 2 的已知输出。不含性能测试 |
| [`.github/workflows/compile.yml`](.github/workflows/compile.yml) | 只做上面的编译检查。不是计时 CI |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/` |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录和阶段报告 |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。Day 4 的转录和程序导出分文件放在同一目录；Day 5 是 30 条程序导出；Day 7 是 75 条分块程序导出 |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |
