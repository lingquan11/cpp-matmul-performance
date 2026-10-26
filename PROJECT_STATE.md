# 我现在做到哪

最近我在 2026-10-26 更新过。i-j-k 小规模正确性（Day 2）有了。Day 4 三个目标这轮也做完了。Day 5 的循环顺序对比（`ijk` 与 `ikj`）这轮验收通过。仓库是 https://github.com/lingquan11/cpp-matmul-performance

我想把 C++ 矩阵乘法做成能复现的实验：先保证算对，再把性能测稳，然后比较循环顺序、做分块、把报告写清楚。测到的数据复用，我再去对 MAC、PE、脉动阵列和 SCALE-Sim。我给自己的日期是 2026-12-10，12 月 11 日之前把材料理好。量化先不放进主线。

Day 1 的 `src/day01_matrix.cpp` 算 C[0][0]，输出 11，笔记在 `notes/day01-notes.md`。Day 2 的 `src/day02_matrix.cpp` 是固定 2×2 的 i-j-k，原始输入输出 `11 10` 和 `19 30`，单位矩阵测试得到 A 本身。阶段 A 的报告在 `experiments/reports/stage-a-algorithm-implementation.md`。Day 3 我把 B 换成零矩阵，输出全 0，换回原来的输入后又是 `11 10` 和 `19 30`。当天的 O(n³) 和实验方案在 `notes/day03-notes.md`。

Day 4 是 2026-10-26 做的。目标①规模扩展、目标②计时入门、目标③完整基准测试，辅导时都过了。正式样本是 n = 64、128、256，每种 5 次，共 15 条。我提交的运行截图里 15 条都是 PASS。128 阶第 5 次明显偏高，原始记录留着，原因还没定。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](src/day04_matmul.cpp)。那 15 条是运行截图逐行转录的，在 [`experiments/results/2026-10-26_day04-local/`](experiments/results/2026-10-26_day04-local/README.md)，短记录在 [`experiments/reports/2026-10-26-day04-local.md`](experiments/reports/2026-10-26-day04-local.md)。实际执行的完整基准源码还没上传，电脑直接导出的原始 CSV 也还没上传。[`src/gpt-templates/day04_benchmark.cpp`](src/gpt-templates/day04_benchmark.cpp) 是 GPT 模板，不是我独立写的，也不保证和我电脑上执行的版本逐字一样。编译器、CPU、是不是 Release/x64，还有当天学习用时，都是未记录。笔记在 [`notes/day04-notes.md`](notes/day04-notes.md)，任务记录在 [`notes/day04-record.md`](notes/day04-record.md)。

从固定 2×2 到单次计时、再到批量基准模板，版本快照在 [`code_history/day04/`](code_history/day04/README.md)。V01 和 V05 是我实际粘贴的；V02、V03、V04、V06 是 GPT 模板，不是我独立写的。要改的程序仍看 `src/`。

同一天接着做了 Day 5，比的是循环顺序，这轮验收通过。我交了完整的双版本程序，`ijk` 和 `ikj` 放在同一次运行里交替测。正式样本是 n = 64、128、256，每种算法 5 次，共 30 条，全部 PASS，逐元素比较全部 YES。CSV 在 [`experiments/results/2026-10-26_day05-loop-order/raw-today-loop-order.csv`](experiments/results/2026-10-26_day05-loop-order/raw-today-loop-order.csv)。按今天这份同轮 CSV，`ijk` 中位数除以 `ikj` 中位数：64 阶 6.85×（0.140724 / 0.020545 ms），128 阶 8.45×（1.375541 / 0.162732 ms），256 阶 9.38×（11.333944 / 1.207955 ms）。源码在 [`src/day05_ijk_ikj_benchmark.cpp`](src/day05_ijk_ikj_benchmark.cpp)，按我提交的内容原样放着，里面不一定每一行都是我自己写的。昨天我另外交过只测 `ikj` 的程序和 15 条 PASS，CSV 在同目录的 `raw-yesterday-ikj-only.csv`。那是另一天、另一份程序，可以并列留着，不当成和今天同一轮的算法对照。[`src/gpt-templates/day05_loop_order_comparison.cpp`](src/gpt-templates/day05_loop_order_comparison.cpp) 是 GPT 模板，不是我独立写的。CPU 型号、编译器具体版本、学习用时仍是未记录。配置截图显示 Visual Studio Release | x64。笔记在 [`notes/day05-notes.md`](notes/day05-notes.md)，任务记录在 [`notes/day05-record.md`](notes/day05-record.md)，短记录在 [`experiments/reports/2026-10-26-day05-loop-order.md`](experiments/reports/2026-10-26-day05-loop-order.md)。

昨天的单算法和今天的双版本，快照在 [`code_history/day05/`](code_history/day05/README.md)。今天这份和 `src/day05_ijk_ikj_benchmark.cpp` 是同一份复制。`src/day04_matmul.cpp` 没动。

下一步我学缓存行和空间局部性，并把现在的 `ijk`、`ikj` 留作分块的基线。Day 4 的编译器、CPU、是不是 Release/x64，以及那次完整基准源码和原始 CSV，仍然没补上；那组和 Day 5 分开记。若补测 Day 4 的 128 阶，旧样本留着，补测另记一条。以后各版本的输入、正确性检查、计时范围和编译设置保持一致。循环顺序的原始数据留着。cache miss 和 TLB miss 等我测完大规模后再对。再往后是脉动阵列和 SCALE-Sim。计时驱动等我自己测稳后再放进 `benchmarks/`。现在那个目录只有占位说明，没有可跑的云端基准脚本。

时间上我每周大约留 8 到 10 小时，课优先。大约七到八成排成预定的任务，剩下的用来排错和复习。工作日做小步，完整实验等我有整块时间。一件事做到两小时还卡住，我就先把卡点记下来再拆开。

测量上，一开始我打算测 64、128、256，每种 5 次。Day 4 本地正式样本停在 256。单次太短就在同一样本里重复，并把换算写下来。每一次原始耗时都留着，表里用中位数。计时范围、输入怎么初始化、清零、打印和正确性检查放在哪，写在实验记录里。朴素版、循环顺序和分块用同一口径，口径变了就另记一次实验。表上的耗时和加速比都对得上某一次测量。

2026-10-25 我定的计时口径是：计时含结果矩阵清零和矩阵乘法，输入构造、打印和正确性检查在外面。Day 4 本地正式样本也是这套。Day 5 昨天和今天的程序也是这套。Day 3 的方案把清零放在计时外，和这两天分开读。

有新的测量或代码，我就更新这份文件。12 月每次有进展也更新，当天没进展就写「无新进展」。README 我留短一点，链到这里。细节在 `notes/`、`experiments/results/` 和 `experiments/reports/`。

Day 1 我花了 30 分钟，Day 2 大约 20 分钟，Day 3 没记时间，Day 4 和 Day 5 的学习用时都未记录。Day 4 本地那组没有对应的环境记录。Day 5 的 CPU 型号和编译器具体版本也未记录；配置截图显示 Visual Studio Release | x64。

10 月 25 日我定下来：先把核心实验做扎实，架构上的衔接放进主线，SCALE-Sim 做一个基础实验。测量分步加，正确性也分步查，计划按周滚动。这份状态就是那天开始写的。12 月 10 日是目标，12 月 11 日前把材料理好。当天怎么定的，我另外记在 `notes/handoff-2026-10-25-项目变更与协作同步报告.md`。
