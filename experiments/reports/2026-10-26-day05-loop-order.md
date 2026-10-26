# 我的实验记录：Day 5 循环顺序

2026-10-26。我是东南大学电子信息大一。这页记 `ijk` 和 `ikj`。今天的 CSV 在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../results/2026-10-26-day05-loop-order-local/README.md)，前一轮的 CSV 在 [`experiments/results/2026-10-25-day05-ikj-only-prior/`](../results/2026-10-25-day05-ikj-only-prior/README.md)。任务记录在 [`notes/day05-record.md`](../../notes/day05-record.md)，学习笔记在 [`notes/day05-notes.md`](../../notes/day05-notes.md)。

今天和前一轮各一份 CSV，都是原样放进来的，测量数字没有改。两份不是同一轮，程序结构也不同。下面分开写。不把前一轮的单算法耗时和今天的双版本耗时做成同一轮对照。前一轮也不是 Day 4 的 ijk 基线。

## 今天：同一次运行里的 ijk 和 ikj

程序是我提交的 [`src/day05_ijk_ikj_benchmark.cpp`](../../src/day05_ijk_ikj_benchmark.cpp)。输入按公式生成：`A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`。

计时包住 C 的清零和矩阵乘法。A、B 的初始化、校验和、逐元素比较、输出都在计时外面。参考矩阵用 `ijk` 试测的结果来存。奇数样本先测 `ijk`，偶数样本先测 `ikj`。

正式样本是 64、128、256。每种算法 5 次，一共 30 条。30 条都是 PASS，逐元素比较都是 YES。同一规模的 `C[0][0]` 和预期一致，校验和也一致：64 阶 `C[0][0] = 176800`，128 阶 `1406272`，256 阶 `11217536`。

每个样本的重复次数：64 阶 `ijk` 613、`ikj` 3449；128 阶 `ijk` 75、`ikj` 570；256 阶 `ijk` 9、`ikj` 80。单次耗时是这一次样本的总毫秒除以重复次数。

中位数和下面的均值、最小、最大，都只从今天这份 [`day05-loop-order-program-export.csv`](../results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv) 的 `ms_per_multiply` 来。每组 5 个数。比值是 `ijk` 中位数除以 `ikj` 中位数，写到两位小数。均值写到和 CSV 一样的 6 位小数。程序写出的原文件名是 `matmul_benchmark_day5_loop_order.csv`。

| n | ijk 中位数 (ms) | ikj 中位数 (ms) | 中位数耗时比（ijk/ikj） |
| ---: | ---: | ---: | ---: |
| 64 | 0.140724 | 0.020545 | 6.85× |
| 128 | 1.375541 | 0.162732 | 8.45× |
| 256 | 11.333944 | 1.207955 | 9.38× |

| n | 算法 | 均值 (ms) | 最小值 (ms) | 最大值 (ms) |
| ---: | --- | ---: | ---: | ---: |
| 64 | ijk | 0.140169 | 0.137043 | 0.143076 |
| 64 | ikj | 0.020967 | 0.020239 | 0.022894 |
| 128 | ijk | 1.452732 | 1.345808 | 1.774361 |
| 128 | ikj | 0.185144 | 0.155615 | 0.270315 |
| 256 | ijk | 11.327138 | 11.266811 | 11.373856 |
| 256 | ikj | 1.209587 | 1.157249 | 1.266663 |

128 阶 `ijk` 最大 1.774361 ms，`ikj` 最大 0.270315 ms。原始记录留着，原因我没定。

今天这一轮里，`ikj` 的中位数低于 `ijk`。内层 j 连续走，`B[k][j]` 和 `C[i][j]` 更顺着一行，这和按行存储对得上。比值是这次程序、这次规模、这次运行下的结果，不是所有机器上都一样的常数。

配置截图在 [`visual-studio-release-x64.png`](../results/2026-10-26-day05-loop-order-local/visual-studio-release-x64.png)。截图显示 Visual Studio Release | x64。CPU 型号、编译器具体版本、学习用时：未记录。截图没有单独注明只属于今天还是前一轮，原图放在今天的结果目录。

## 前一轮：只测 ikj

文件是 [`day05-ikj-only-program-export.csv`](../results/2026-10-25-day05-ikj-only-prior/day05-ikj-only-program-export.csv)。15 条，64、128、256 各 5 次，全部 PASS。表头没有 `algorithm`，也没有逐元素比较。对应的程序快照是 [`code_history/day05/V01_yesterday_ikj_only.cpp`](../../code_history/day05/V01_yesterday_ikj_only.cpp)，里面只有 `matmul_ikj`。程序写出的原文件名是 `matmul_benchmark_raw.csv`。CSV 内没有日历日期；验收包把这轮叫做相对 2026-10-26 的「昨天」，结果目录因此用 2026-10-25，这不是 CSV 里的时间戳。

重复次数是 64 阶 2208、128 阶 552、256 阶 84。计时同样包住 C 清零和乘法。检查是 `C[0][0]` 和校验和，没有今天那种逐元素列。

这 15 条和今天的 30 条分开放。我不拿它们对 `ijk` 有多快，也不把两轮的单次耗时排成一张算法对照表。它也不是 Day 4 那份截图转录。

## 代码放在哪

1. 我提交的今天双版本：[`src/day05_ijk_ikj_benchmark.cpp`](../../src/day05_ijk_ikj_benchmark.cpp)。快照 [`code_history/day05/V02_today_ijk_ikj.cpp`](../../code_history/day05/V02_today_ijk_ikj.cpp) 是同一份复制。里面不一定每一行都是我自己写的。`matmul_ikj` 的累加后面还留着 TODO 注释和多出来的分号，可以以后删，这次结果不变。
2. 我此前提交的昨天单算法：只在 `code_history/day05/`。没有拿它盖掉今天的 `src/` 文件，也没有动 `src/day04_matmul.cpp`。
3. [`templates/gpt/day05-loop-order-template.cpp`](../../templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，文件头写了「GPT 指导模板」。它不是我交上来的源码，也不保证和我电脑上跑的版本逐字一样。验收包原件在 [`archive/2026-10-26-day05-final-acceptance/`](../../archive/2026-10-26-day05-final-acceptance/README.md)。

## 材料缺口

1. CPU 型号：未记录。
2. 编译器具体版本：未记录。源码里的泛型 lambda 需要 C++14 或更新的标准，这是语言要求，不是我记下来的编译器版本。
3. 学习用时：未记录。
4. 配置截图只支持一条：Visual Studio Release | x64。操作系统没有另记。

## 下一步

1. 看缓存行和空间局部性。
2. 把现在的 `ijk` 和 `ikj` 留作分块的基线。
3. 以后各版本的输入、正确性检查、计时范围和编译设置保持一致。口径变了就另记一次，不覆盖这两份 CSV。
