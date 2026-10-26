# 我现在做到哪

最后更新：2026-10-26。用户确认原先误标成 Day 5「昨天 15 条」的 CSV 是 Day 4 程序导出，已改回 Day 4 目录。Day 5 只保留今天的 30 条。测量数字没有改。无新的计时实验。同一天把 `src/` 现行文件改成连字符文件名，并加上只编译、只冒烟 Day 1 / Day 2 的检查。Day 4、Day 5 计时复现：未验证。验收 ZIP 原件不再入库。i-j-k 小规模正确性（Day 2）有了。Day 4 三个目标这轮也做完了。Day 5 的循环顺序对比（`ijk` 与 `ikj`）已验收通过。仓库是 https://github.com/lingquan11/cpp-matmul-performance

我想把 C++ 矩阵乘法做成能复现的实验：先保证算对，再把性能测稳，然后比较循环顺序、做分块、把报告写清楚。测到的数据复用，我再去对 MAC、PE、脉动阵列和 SCALE-Sim。我给自己的日期是 2026-12-10，12 月 11 日之前把材料理好。量化先不放进主线。

Day 1 的 `src/day01-matrix.cpp` 算 C[0][0]，输出 11，笔记在 `notes/day01-notes.md`。Day 2 的 `src/day02-matrix.cpp` 是固定 2×2 的 i-j-k，原始输入输出 `11 10` 和 `19 30`，单位矩阵测试得到 A 本身。阶段 A 的报告在 `experiments/reports/stage-a-algorithm-implementation.md`。Day 3 我把 B 换成零矩阵，输出全 0，换回原来的输入后又是 `11 10` 和 `19 30`。当天的 O(n³) 和实验方案在 `notes/day03-notes.md`。

Day 4 是 2026-10-26 做的。目标①规模扩展、目标②计时入门、目标③完整基准测试，辅导时都过了。同一天留着两份 15 条，都是 PASS，数值不同，不合并。截图转录在 [`experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv`](experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv)，重复次数 585 / 89 / 9，耗时在后来 `ijk` 的量级。程序导出在同目录的 [`day04-local-program-export.csv`](experiments/results/2026-10-26-day04-local/day04-local-program-export.csv)，重复次数 2208 / 552 / 84，耗时更快。用户确认这份导出就是今日的 Day 4，不是 Day 5。128 阶偏高样本保留原值，原因暂留，以后再研究。截图转录里是第 5 次 2.010587 ms。我粘贴过的单次计时程序在 [`src/day04-matmul.cpp`](src/day04-matmul.cpp)。截图转录那组的批量源码还没上传。[`templates/gpt/day04-benchmark-template.cpp`](templates/gpt/day04-benchmark-template.cpp) 是 GPT 模板，不是执行稿。和程序导出一起提交的源码在 [`code_history/day04/V07_user_ikj_program_export.cpp`](code_history/day04/V07_user_ikj_program_export.cpp)，只含 `matmul_ikj`，原先误放在 `code_history/day05/`。表头一致，没有重跑，不写成已经逐行复现。短记录在 [`experiments/reports/2026-10-26-day04-local.md`](experiments/reports/2026-10-26-day04-local.md)。环境是用户补记的：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x；配置截图显示 Release | x64。更细的编译器版本号未记录。学习用时补记为 2 小时。笔记在 [`notes/day04-notes.md`](notes/day04-notes.md)，任务记录在 [`notes/day04-record.md`](notes/day04-record.md)。

从固定 2×2 到单次计时、再到批量基准模板，版本快照在 [`code_history/day04/`](code_history/day04/README.md)。V01 和 V05 是我实际粘贴的；V02、V03、V04、V06 是 GPT 模板，不是我独立写的；V07 是和 Day 4 程序导出一起提交的 `ikj` 批量程序。`code_history/dayNN/` 记单日代码变化的快照，`src/` 是这一阶段的现行成品。两处并存是正常的。哈希不同的文件不合成一份。快照文件名保持原样；要改的程序仍看 `src/` 里的连字符路径。

同一天接着做了 Day 5，比的是循环顺序，已验收通过。验收只看今天的双版本。我交了完整的双版本程序，`ijk` 和 `ikj` 放在同一次运行里交替测。正式样本是 n = 64、128、256，每种算法 5 次，共 30 条，全部 PASS，逐元素比较全部 YES。程序导出的 CSV 在 [`experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv`](experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv)。按今天这份同轮 CSV，`ijk` 中位数除以 `ikj` 中位数：64 阶 6.85×（0.140724 / 0.020545 ms），128 阶 8.45×（1.375541 / 0.162732 ms），256 阶 9.38×（11.333944 / 1.207955 ms）。128 阶偏高的单次保留原值，原因暂留，以后再研究。源码在 [`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp)，按我提交的内容原样放着，里面不一定每一行都是我自己写的。快照 [`code_history/day05/V02_today_ijk_ikj.cpp`](code_history/day05/V02_today_ijk_ikj.cpp) 是同一份复制。[`templates/gpt/day05-loop-order-template.cpp`](templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是我独立写的。环境与 Day 4 一致：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x，配置截图显示 Release | x64。更细的编译器版本号和学习用时仍未记录。材料已按职责归档进 src / notes / experiments，验收 ZIP 原件不再入库。笔记在 [`notes/day05-notes.md`](notes/day05-notes.md)，任务记录在 [`notes/day05-record.md`](notes/day05-record.md)，短记录在 [`experiments/reports/2026-10-26-day05-loop-order.md`](experiments/reports/2026-10-26-day05-loop-order.md)。`src/day04-matmul.cpp` 没动。

下一步我学缓存行和空间局部性，并把现在的 `ijk`、`ikj` 留作分块的基线。Day 4 截图转录那组的批量源码仍然没上传。若补测 128 阶，旧样本留着，补测另记一条。以后各版本的输入、正确性检查、计时范围和编译设置保持一致。循环顺序的原始数据留着。cache miss 和 TLB miss 等我测完大规模后再对。再往后是脉动阵列和 SCALE-Sim。

`benchmarks/` 保持占位。正式可跑的计时驱动等我本地测稳后再放进去。现在没有云端可跑基准，也不恢复以前删掉的计时脚手架。`.github/workflows/compile.yml` 只做一件事：用 `g++ -std=c++17 -O2` 编译 `src/` 下现存的 `.cpp`，并用 [`tests/smoke.sh`](tests/smoke.sh) 检查 Day 1 输出 `11`、Day 2 输出 `11 10` 和 `19 30`。这不是完整测试矩阵，也不是性能 CI。Day 4 和 Day 5 的计时复现：未验证。不在 CI 或云端重跑，以免混入新样本。

材料缺口：待用户提供。虚拟机或大规模扫描的结果不在库里，不能凭空补 CSV 或报告。另外仍缺的是：Day 4 截图转录那组的批量源码；比 Visual Studio 2026 18.x 更细的编译器版本号；Day 5 学习用时；128 阶偏高样本的原因。

往后加循环重排、分块、缓存分析、MAC、PE、脉动阵列或 SCALE-Sim 时，沿用现在的位置：我提交的程序放 `src/`，GPT 模板放 `templates/gpt/`，历史快照放 `code_history/dayNN/`，一组实测放 `experiments/results/YYYY-MM-DD-<experiment>-<environment>/` 并写 `metadata.md`，正式报告放 `experiments/reports/`，理解笔记和当日任务放 `notes/dayNN-notes.md` 与 `notes/dayNN-record.md`，再在 `notes/daily-task-records.md` 加索引。不同日期、机器或计时口径分开目录，不覆盖旧 CSV。验收过后更新本文件；当天没有新测量或新代码，就写「无新进展」。

时间上我每周大约留 8 到 10 小时，课优先。大约七到八成排成预定的任务，剩下的用来排错和复习。工作日做小步，完整实验等我有整块时间。一件事做到两小时还卡住，我就先把卡点记下来再拆开。

测量上，一开始我打算测 64、128、256，每种 5 次。Day 4 本地正式样本停在 256。单次太短就在同一样本里重复，并把换算写下来。每一次原始耗时都留着，表里用中位数。计时范围、输入怎么初始化、清零、打印和正确性检查放在哪，写在实验记录里。朴素版、循环顺序和分块用同一口径，口径变了就另记一次实验。表上的耗时和加速比都对得上某一次测量。

2026-10-25 我定的计时口径是：计时含结果矩阵清零和矩阵乘法，输入构造、打印和正确性检查在外面。Day 4 的截图转录和程序导出，以及 Day 5 的双版本，都是这套。用户确认 Day 4 与 Day 5 的实验因素保持一致。Day 3 的方案把清零放在计时外，和这两天分开读。

有新的测量或代码，我就更新这份文件。12 月每次有进展也更新，当天没进展就写「无新进展」。README 我留短一点，链到这里。细节在 `notes/`、`experiments/results/` 和 `experiments/reports/`。

Day 1 我花了 30 分钟，Day 2 大约 20 分钟。Day 3 当天没写用时，2026-10-26 补记为 30 分钟。Day 4 补记为 2 小时。Day 5 的学习用时仍未记录。Day 4 和 Day 5 的环境是用户补记的同一套：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x。配置截图显示 Release | x64。比 18.x 更细的编译器版本号未记录。

10 月 25 日我定下来：先把核心实验做扎实，架构上的衔接放进主线，SCALE-Sim 做一个基础实验。测量分步加，正确性也分步查，计划按周滚动。这份状态就是那天开始写的。12 月 10 日是目标，12 月 11 日前把材料理好。当天怎么定的，我另外记在 `notes/handoff-2026-10-25-项目变更与协作同步报告.md`。
