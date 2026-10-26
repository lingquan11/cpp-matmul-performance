# cpp-matmul-performance

[![小规模基准](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml/badge.svg)](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml)

用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 重要。

我是东南大学电子信息专业大一。

## 当前做到哪

i-j-k 小规模正确性（Day 2）已有。`src/day02_matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。

Day 4 本地 64、128、256 的截图转录已归档，在 [`experiments/results/2026-10-26_day04-local/`](experiments/results/2026-10-26_day04-local/README.md)。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](src/day04_matmul.cpp)。实际执行的完整基准源码和电脑直接导出的原始 CSV 还没上传。[`src/gpt-templates/`](src/gpt-templates/README.md) 里放的是 GPT 模板，不是我独立写的。从固定 2×2 到批量模板的版本快照在 [`code_history/day04/`](code_history/day04/README.md)，和 `src/` 现行文件并存，哈希未必相同。

公开展示用的规模扫描结果已按我的要求撤下。等我自己在本地或正式复现之后，再放。

下一步我先把 Day 4 的编译器和构建配置补记清楚。若补测 128，旧样本留着另记。然后加另一种循环顺序，再在同一口径下做分块，然后看脉动阵列和 SCALE-Sim。

眼下的安排以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准。

## 怎么跑

```bash
python3 benchmarks/run_bench.py
```

需要 `g++` 和 Python 3。画图需要 `matplotlib`；没有安装时这条命令会尝试安装。默认规模 64、128、256、512、1024，每个规模 5 个正式样本，输出到 `experiments/results/<日期>_matmul/`。

脚本会把 `benchmarks/kernels/` 里的实现画在同一张图上，现在里面是 i-j-k。循环重排或分块放进这个目录之后，还是跑同一条命令，步骤在 [`benchmarks/README.md`](benchmarks/README.md)。

推送到 `main` 时，上面的徽章会编译并跑一次 n=64，只检查能否通过。这次检查不往仓库里写结果。

## 结果在哪

现在公开留着的是 Day 4 本地 64、128、256 的截图转录，在 [`experiments/results/2026-10-26_day04-local/`](experiments/results/2026-10-26_day04-local/README.md)。短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。

报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。上面说的规模扫描已撤下，这里不再放那次的表和图。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 4 在 [`notes/day04-notes.md`](notes/day04-notes.md)
- 每日归档 `dayNN-record.md`，Day 4 在 [`notes/day04-record.md`](notes/day04-record.md)
- 每天的流水 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 学习程序（Day 1 单元素，Day 2 的 2×2，Day 4 单次计时）。[`src/gpt-templates/`](src/gpt-templates/README.md) 只放 GPT 模板 |
| [`code_history/`](code_history/README.md) | Day 4 代码演进快照（V01–V06）。和 `src/` 现行文件并存，哈希未必相同 |
| [`benchmarks/`](benchmarks/README.md) | 计时驱动。`kernels/` 放要上图的实现 |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/` |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录与阶段报告（唯一报告入口） |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。现在留着的是 Day 4 本地截图转录 |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |

<details>
<summary>协作备忘（看成果可以跳过）</summary>

2026-10-25 确认的范围和教学约定在 [`notes/handoff-2026-10-25-项目变更与协作同步报告.md`](notes/handoff-2026-10-25-项目变更与协作同步报告.md)。那是给后续协作者的决定记录，不是实验结论。

</details>
