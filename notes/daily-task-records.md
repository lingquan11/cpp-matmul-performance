# 我做完的每天

这里是每天的索引。已经做完并且有反馈的任务才记进来。整理是从 2026-10-22 开始的。没做完的我写在当天的下一步里，不记成已经完成。到现在按日期记了 5 个日历日：2026-10-20、2026-10-22、2026-10-23、2026-10-26、2026-10-27。2026-10-26 这一天里 Day 4 和 Day 5 都做完了。2026-10-27 这一天里 Day 6 和 Day 7 都做完了。当天的细节以 `dayNN-record.md` 和 `dayNN-notes.md` 为准，实验数字以 `experiments/` 为准，当前状态以 `PROJECT_STATE.md` 为准。

| 日期 | Day | 入口 |
| --- | --- | --- |
| 2026-10-20 | Day 1 | [day01-notes.md](day01-notes.md) |
| 2026-10-22 | Day 2 | [day02-record.md](day02-record.md) |
| 2026-10-23 | Day 3 | [day03-notes.md](day03-notes.md)、[day03-record.md](day03-record.md) |
| 2026-10-26 | Day 4 | [day04-notes.md](day04-notes.md)、[day04-record.md](day04-record.md)、[转录和程序导出](../experiments/results/2026-10-26-day04-local/README.md)、[源码缺口](supplements/day04-screenshot-source-gap.md)、[128 阶分析](supplements/day04-day05-n128-high-samples.md) |
| 2026-10-26 | Day 5，已验收，只计 30 条，用时 60 分钟 | [day05-notes.md](day05-notes.md)、[day05-record.md](day05-record.md)、[今天 30 条](../experiments/results/2026-10-26-day05-loop-order-local/README.md)、[用时补录](supplements/day05-study-time.md) |
| 2026-10-27 | Day 6，概念验收，无新代码 | [day06-notes.md](day06-notes.md)、[day06-record.md](day06-record.md)、[day06-code-materials.md](day06-code-materials.md) |
| 2026-10-27 | Day 7，分块验收，75 条；src 中为 GPT 整合版 | [day07-notes.md](day07-notes.md)、[day07-record.md](day07-record.md)、[day07-code-materials.md](day07-code-materials.md)、[75 条](../experiments/results/2026-10-27-day07-blocking-local/README.md)、[环境](supplements/experiment-environment.md) |
| 未记录 | Day 8，性能诊断综合考核通过，无新代码、无 CSV，用时 300 分钟 | [day08-notes.md](day08-notes.md)、[day08-record.md](day08-record.md)、[对话](day08-dialogue-record.md)、[原话](day08-visible-user-turns.md)、[day08-code-materials.md](day08-code-materials.md)、[方法报告](../experiments/reports/day08-performance-analysis.md)、[完整性检查](supplements/day08-completeness-check.md) |

我先学会了算一个输出元素，并写成 C++。2026-10-22 我把完整的 2×2 三重循环做完了，原始输入和单位矩阵都通过。阶段 A 到这里。2026-10-23 我做了零矩阵，写了 O(n³)，也写了后面怎么测性能的方案。2026-10-26 我另外做完了 Day 4：规模可以输入，单次计时也接上了，本地 64、128、256 各测了 5 次。截图转录和程序导出都在 [`experiments/results/2026-10-26-day04-local/`](../experiments/results/2026-10-26-day04-local/README.md)。同一天 Day 5 比了循环顺序，验收只看今天的 30 条，在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../experiments/results/2026-10-26-day05-loop-order-local/README.md)。2026-10-27 的 Day 6 过了概念验收，记的是内存布局和缓存局部性。同一天 Day 7 分块验收通过，正式 CSV 75 条全部 PASS，边界测试 `n=70`、`block_size=32` 也过了。

## 2026-10-20

我想从矩阵里的一个元素走到 C++ 的累加。我看了 3Blue1Brown 的 Matrix multiplication as composition，看行和列怎么对上。

矩阵是

```text
A = [[2, 1],
     [3, 4]]

B = [[5, 2],
     [1, 6]]
```

我要算 `C = AB` 里的 `C₁₁` 和 `C₂₁`，说清用了 A 的哪一行、B 的哪一列，用中文把步骤写下来，再写成伪代码。累加变量为什么从 0 开始，k 怎么把对应元素走完，这些我都要能讲。然后在下面这个框架里把累加补上，存成 `day01-matrix.cpp`，编译运行，和手算对一下。还要看懂 `A[0][k]`、`B[k][0]`，知道为什么是累加不是覆盖，以及 C++ 下标为什么从 0 开始。手算、伪代码和解释存成 `day01-notes.md`。

开始时的框架是这样，我补的是循环里的那句累加。完整程序在 `src/day01-matrix.cpp`。

```cpp
#include <iostream>
using namespace std;

int main() {
    int A[2][2] = {{2, 1}, {3, 4}};
    int B[2][2] = {{5, 2}, {1, 6}};
    int c00 = 0;

    for (int k = 0; k < 2; ++k) {
        // 将对应元素的乘积累加到 c00
    }

    cout << c00 << endl;
    return 0;
}
```

我手算 `C₁₁ = 2 × 5 + 1 × 1 = 11`，`C₂₁ = 3 × 5 + 4 × 1 = 19`。程序我补上之后跑通了。2026-10-22 用 g++ 再编译运行，输出 11，和手算一致。笔记在 `notes/day01-notes.md`。我当时回复的是结果 `11，19`，用时 30 分钟。

这一天我确认了三件事：能手算一个输出元素，能按左边一行和右边一列把元素挑出来，能把这个单元素程序补完并跑起来。完整矩阵的三重循环是第二天的事。

下一步我打算用 `i/j/k` 把完整的 2×2 写出来。

## 2026-10-22

在单元素的基础上，我用 `i/j/k` 写了完整的 2×2，并用原始矩阵和单位矩阵各测一次。代码在 `src/day02-matrix.cpp`，更全的记录在 `notes/day02-record.md`。当天我也把阶段 A 的记录理了一遍。

`i` 选结果的行，`j` 选结果的列，`k` 把要相乘的位置走完。`C[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j]`。单位矩阵是用来看右乘之后 A 还是不是自己。

原始输入输出 `11 10` 和 `19 30`，和手算一致。单位矩阵输出 `2 1` 和 `3 4`，就是 A。2026-10-22 我用 g++（C++17, -O2）编译运行，两组都和记录一致。大约用了 20 分钟。

下一步我去学怎么计时、怎么重复测、哪些东西必须保持一样，这样后面才能比循环顺序，才能看缓存和数据局部性。

## 2026-10-23

我在 Day 2 的程序上做了零矩阵，推了三重循环的 O(n³)，也说明了边长翻倍时计算量怎么变，并写下性能方案：64、128、256，Release，每个规模 5 次，只计矩阵乘法本身。笔记在 `notes/day03-notes.md`，当天的短说明在 `notes/day03-record.md`。程序还是 `src/day02-matrix.cpp`。当天没写用时，后来补记为 30 分钟。

B 是零矩阵时，输出是零矩阵。换回原始输入后是 `11 10` 和 `19 30`，和 Day 2 一样。

三层循环每层各走 n 次。i 和 j 的组合有 n² 个，每个结果元素要 n 次乘加，最内层一共 n³ 次。朴素方阵乘法是 O(n³)，也可以记成 Θ(n³)。边长翻倍，计算量变成原来的 8 倍。这是计算量。耗时倍数等我测完再写。

下一步我按方案去比不同规模的耗时，然后再做循环重排。

## 2026-10-26

Day 4 三个目标这轮都做完了。程序可以输入矩阵阶数，非法的 n 会拒绝。我用 `steady_clock` 给单次乘法计了时。正式样本是 64、128、256 各 5 次，截图里 15 条都是 PASS。笔记在 [`notes/day04-notes.md`](day04-notes.md)，任务记录在 [`notes/day04-record.md`](day04-record.md)，短实验记录在 [`experiments/reports/2026-10-26-day04-local.md`](../experiments/reports/2026-10-26-day04-local.md)。学习用时后来补记为 2 小时。环境沿用已提交记录：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。Day 4–Day 7 同机，未换机。

我粘贴过的单次计时程序在 [`src/day04-matmul.cpp`](../src/day04-matmul.cpp)。容量 `MAX_N = 256`，A 和 B 用公式填。n = 1、2、3、4 我看过，结果符合预期。n = 0 和 n = 257 会被拒绝。n = 0 是循环进不去；n = 257 是超过数组容量，不检查就可能越界。

计时包住 C 清零和乘法。A、B 初始化、输出和正确性检查在外面。初步试过 n = 4，`C[0][0] = 50`，0.0016 ms；n = 64，`C[0][0] = 176800`，0.4099 ms。这两条不是正式样本。

截图里的 15 条我逐行抄进 [`experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv`](../experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv)。这是转录，不是程序导出。程序导出在同目录的 [`day04-local-program-export.csv`](../experiments/results/2026-10-26-day04-local/day04-local-program-export.csv)，数值不同，下面这张表不含它。截图转录那组的实际完整 benchmark 源码仍缺。[`templates/gpt/day04-benchmark-template.cpp`](../templates/gpt/day04-benchmark-template.cpp) 和 V06 是 GPT 模板，不能冒充实际执行源码。说明在 [`supplements/day04-screenshot-source-gap.md`](supplements/day04-screenshot-source-gap.md)。和程序导出一起提交的源码在 [`code_history/day04/V07_user_ikj_program_export.cpp`](../code_history/day04/V07_user_ikj_program_export.cpp)。

| n | 样本 | 重复次数 | 单次中位数 (ms) | 单次均值 (ms) |
| ---: | ---: | ---: | ---: | ---: |
| 64 | 5 | 585 | 0.141690 | 0.142039 |
| 128 | 5 | 89 | 1.204964 | 1.396575 |
| 256 | 5 | 9 | 10.891122 | 10.859920 |

128 阶第 5 次单次 2.010587 ms，总时间 178.942200 ms，比同规模其他几次高。原值留着。程序导出里另有第 3 次 0.189740 ms。分析在 [`supplements/day04-day05-n128-high-samples.md`](supplements/day04-day05-n128-high-samples.md)，没有删点，也没有确定唯一根因。

如果补测 128，旧样本留着，补测另记。然后继续看循环顺序和数据访问，再比分块。以后各版本的输入、正确性检查、计时范围和编译设置保持一致。

## 2026-10-26，Day 5

循环顺序这轮验收通过，只看今天的 30 条。学习用时补录为 60 分钟。环境和 Day 4 是同一套：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。

我提交的双版本在 [`src/day05-ijk-ikj-benchmark.cpp`](../src/day05-ijk-ikj-benchmark.cpp)。`ijk` 和 `ikj` 同一次运行，奇数样本先 `ijk`，偶数样本先 `ikj`。计时包住 C 清零和乘法。输入初始化、校验和、逐元素比较在外面。笔记在 [`notes/day05-notes.md`](day05-notes.md)，任务记录在 [`notes/day05-record.md`](day05-record.md)，短实验记录在 [`experiments/reports/2026-10-26-day05-loop-order.md`](../experiments/reports/2026-10-26-day05-loop-order.md)。

今天的 CSV 是我交的双版本测试结果，30 条，64、128、256，每种算法 5 次，全部 PASS，逐元素全部 YES。文件在 [`experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv`](../experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv)。

| n | ijk 中位数 (ms) | ikj 中位数 (ms) | ijk/ikj |
| ---: | ---: | ---: | ---: |
| 64 | 0.140724 | 0.020545 | 6.85× |
| 128 | 1.375541 | 0.162732 | 8.45× |
| 256 | 11.333944 | 1.207955 | 9.38× |

这些中位数只从今天这一份 CSV 来。曾经被写成「昨天 15 条」的文件，用户确认是 Day 4 程序导出，不在这张表里。

[`templates/gpt/day05-loop-order-template.cpp`](../templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是我独立写的。

`ijk` 内层走 B 的列，按行存时跳得大。`ikj` 内层 j 连续，B 和 C 更顺着行走。C 要先清零再累加，不能在每次 k 里重新清零。

下一步我看缓存行和空间局部性，并把这两个版本留作分块基线。

## 2026-10-27，Day 6

今天我学的是二维数组在内存里怎么排、缓存行、空间局部性，以及循环重排和分块差在哪里。四个目标都完成了：按行优先算元素地址；连续访问和跨行访问跟缓存行的关系；运行时间差不能单独证明缓存未命中少了；循环重排改的是访问顺序，分块是把大矩阵拆成小块，让正在用的数据更有机会留在缓存里被反复用到。

今天没有新的 C++ 程序，也没有新的 CSV。笔记在 [`day06-notes.md`](day06-notes.md)，任务记录在 [`day06-record.md`](day06-record.md)，没有新代码这件事写在 [`day06-code-materials.md`](day06-code-materials.md)。

下一步我学分块矩阵乘法的基本结构，先用小规模示意把子块的索引推清楚，再决定要不要写代码去测。Day 5 的 `ijk` 和 `ikj` 还留着当基线。

## 2026-10-27，Day 7

同一天接着做了分块矩阵乘法。学习验收通过，学习用时 60 分钟。环境与 Day 4–Day 6 同机：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。

正式样本是 64、128、256，五种配置各 5 次，共 75 条，全部 PASS，逐元素全部 YES。程序导出在 [`experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv`](../experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv)。边界测试 `n=70`、`block_size=32` 通过，截图在同目录。中位数和分析在 [`experiments/reports/2026-10-27-day07-blocking.md`](../experiments/reports/2026-10-27-day07-blocking.md)：`blocked_64` 三种分块最快；128/256 仍慢于 `ikj`；耗时不能证明 cache miss。

我交的中间版在 [`code_history/day07/V01_user_submitted_intermediate.cpp`](../code_history/day07/V01_user_submitted_intermediate.cpp)，按原样保留。[`src/day07-matrix-blocking.cpp`](../src/day07-matrix-blocking.cpp) 是根据学习对话整理的 GPT 整合版，不是用户本机逐字源码，也没有拿它覆盖正式 CSV。[`templates/gpt/day07-blocking-snippets.md`](../templates/gpt/day07-blocking-snippets.md) 是 GPT 片段。笔记在 [`day07-notes.md`](day07-notes.md)，任务记录在 [`day07-record.md`](day07-record.md)，材料说明在 [`day07-code-materials.md`](day07-code-materials.md)。

下一步可选补边界组合，比较中位数和波动，并学数据复用与分块大小。Day 5 的 `ijk`、`ikj` 仍作基线。

这 5 个日历日里，Day 1 是 30 分钟，Day 2 大约 20 分钟，Day 3 补记为 30 分钟，Day 4 补记为 2 小时，Day 5 补录为 60 分钟，Day 6 是 20 分钟，Day 7 是 60 分钟。2026-10-27 同一天做完了 Day 6 和 Day 7。Day 6 没有新的测量，也没有新的程序。Day 7 有 75 条正式样本和边界截图。`src/day07-matrix-blocking.cpp` 是 GPT 整合版，不是用户逐字源码。单元素程序编译运行输出 11。2×2 的原始输入和单位矩阵我都编译跑过。零矩阵输出是零矩阵，换回去和 Day 2 一致。O(n³) 以及“边长翻倍变成 8 倍”是我推出来的。Day 4 有截图转录和程序导出两份 15 条。截图转录那组的实际批量源码仍缺。Day 5 的正式结果是 2026-10-26 的 30 条。IDE 版本是 Visual Studio 2026 18.11.1。MSVC toolset 独立版本号仍然未记录。Day 4–Day 7 环境沿用 Windows、Intel Core Ultra 7 255HX、Release / x64。补缴索引在 [`supplements/README.md`](supplements/README.md)。

## 日期未记录，Day 8

Day 8 学的是 CPU 性能诊断：缓存行为、MLP、IPC/CPI、Memory Bound、Core Bound、Pipeline Slots、SMT、perf 统计范围和实验设计。综合考核通过，学习用时 5 小时（300 分钟）。归档包没写日期，这里记为未记录。

今天没有新的 C++ 程序，也没有正式 CSV。对话里的 IPC、MLP、带宽、Pipeline Slots 和 A/B/C/D 数字都是教学示例，不是实测。环境沿用此前记录：Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1。对话记录有缺口，聊天里有一段被系统折叠成 “Skipped N messages”，不是逐字无缺口导出。

笔记在 [`day08-notes.md`](day08-notes.md)，任务记录在 [`day08-record.md`](day08-record.md)，对话在 [`day08-dialogue-record.md`](day08-dialogue-record.md)，我的原话在 [`day08-visible-user-turns.md`](day08-visible-user-turns.md)，没有新代码这件事写在 [`day08-code-materials.md`](day08-code-materials.md)。方法报告在 [`experiments/reports/day08-performance-analysis.md`](../experiments/reports/day08-performance-analysis.md)。缺什么在 [`supplements/day08-completeness-check.md`](supplements/day08-completeness-check.md)。

下一步学 SIMD 与向量化。
