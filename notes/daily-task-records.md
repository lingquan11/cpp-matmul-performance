# 我做完的每天

这里是每天的索引。已经做完并且有反馈的任务才记进来。整理是从 2026-10-22 开始的。没做完的我写在当天的下一步里，不记成已经完成。到现在按日期记了 4 天。2026-10-26 这一天里 Day 4 和 Day 5 都做完了。当天的细节以 `dayNN-record.md` 和 `dayNN-notes.md` 为准，实验数字以 `experiments/` 为准，当前状态以 `PROJECT_STATE.md` 为准。

| 日期 | Day | 入口 |
| --- | --- | --- |
| 2026-10-20 | Day 1 | [day01-notes.md](day01-notes.md) |
| 2026-10-22 | Day 2 | [day02-record.md](day02-record.md) |
| 2026-10-23 | Day 3 | [day03-notes.md](day03-notes.md)、[day03-record.md](day03-record.md) |
| 2026-10-26 | Day 4 | [day04-notes.md](day04-notes.md)、[day04-record.md](day04-record.md)、[转录和程序导出](../experiments/results/2026-10-26-day04-local/README.md) |
| 2026-10-26 | Day 5，已验收，只计 30 条 | [day05-notes.md](day05-notes.md)、[day05-record.md](day05-record.md)、[今天 30 条](../experiments/results/2026-10-26-day05-loop-order-local/README.md) |

我先学会了算一个输出元素，并写成 C++。2026-10-22 我把完整的 2×2 三重循环做完了，原始输入和单位矩阵都通过。阶段 A 到这里。2026-10-23 我做了零矩阵，写了 O(n³)，也写了后面怎么测性能的方案。2026-10-26 我另外做完了 Day 4：规模可以输入，单次计时也接上了，本地 64、128、256 各测了 5 次。截图转录和程序导出都在 [`experiments/results/2026-10-26-day04-local/`](../experiments/results/2026-10-26-day04-local/README.md)。同一天 Day 5 比了循环顺序，验收只看今天的 30 条，在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../experiments/results/2026-10-26-day05-loop-order-local/README.md)。

## 2026-10-20

我想从矩阵里的一个元素走到 C++ 的累加。我看了 3Blue1Brown 的 Matrix multiplication as composition，看行和列怎么对上。

矩阵是

```text
A = [[2, 1],
     [3, 4]]

B = [[5, 2],
     [1, 6]]
```

我要算 `C = AB` 里的 `C₁₁` 和 `C₂₁`，说清用了 A 的哪一行、B 的哪一列，用中文把步骤写下来，再写成伪代码。累加变量为什么从 0 开始，k 怎么把对应元素走完，这些我都要能讲。然后在下面这个框架里把累加补上，存成 `day01_matrix.cpp`，编译运行，和手算对一下。还要看懂 `A[0][k]`、`B[k][0]`，知道为什么是累加不是覆盖，以及 C++ 下标为什么从 0 开始。手算、伪代码和解释存成 `day01-notes.md`。

开始时的框架是这样，我补的是循环里的那句累加。完整程序在 `src/day01_matrix.cpp`。

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

在单元素的基础上，我用 `i/j/k` 写了完整的 2×2，并用原始矩阵和单位矩阵各测一次。代码在 `src/day02_matrix.cpp`，更全的记录在 `notes/day02-record.md`。当天我也把阶段 A 的记录理了一遍。

`i` 选结果的行，`j` 选结果的列，`k` 把要相乘的位置走完。`C[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j]`。单位矩阵是用来看右乘之后 A 还是不是自己。

原始输入输出 `11 10` 和 `19 30`，和手算一致。单位矩阵输出 `2 1` 和 `3 4`，就是 A。2026-10-22 我用 g++（C++17, -O2）编译运行，两组都和记录一致。大约用了 20 分钟。

下一步我去学怎么计时、怎么重复测、哪些东西必须保持一样，这样后面才能比循环顺序，才能看缓存和数据局部性。

## 2026-10-23

我在 Day 2 的程序上做了零矩阵，推了三重循环的 O(n³)，也说明了边长翻倍时计算量怎么变，并写下性能方案：64、128、256，Release，每个规模 5 次，只计矩阵乘法本身。笔记在 `notes/day03-notes.md`，当天的短说明在 `notes/day03-record.md`。程序还是 `src/day02_matrix.cpp`。当天没写用时，后来补记为 30 分钟。

B 是零矩阵时，输出是零矩阵。换回原始输入后是 `11 10` 和 `19 30`，和 Day 2 一样。

三层循环每层各走 n 次。i 和 j 的组合有 n² 个，每个结果元素要 n 次乘加，最内层一共 n³ 次。朴素方阵乘法是 O(n³)，也可以记成 Θ(n³)。边长翻倍，计算量变成原来的 8 倍。这是计算量。耗时倍数等我测完再写。

下一步我按方案去比不同规模的耗时，然后再做循环重排。

## 2026-10-26

Day 4 三个目标这轮都做完了。程序可以输入矩阵阶数，非法的 n 会拒绝。我用 `steady_clock` 给单次乘法计了时。正式样本是 64、128、256 各 5 次，截图里 15 条都是 PASS。笔记在 [`notes/day04-notes.md`](day04-notes.md)，任务记录在 [`notes/day04-record.md`](day04-record.md)，短实验记录在 [`experiments/reports/2026-10-26-day04-local.md`](../experiments/reports/2026-10-26-day04-local.md)。学习用时后来补记为 2 小时。环境后来补记为 Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x。

我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](../src/day04_matmul.cpp)。容量 `MAX_N = 256`，A 和 B 用公式填。n = 1、2、3、4 我看过，结果符合预期。n = 0 和 n = 257 会被拒绝。n = 0 是循环进不去；n = 257 是超过数组容量，不检查就可能越界。

计时包住 C 清零和乘法。A、B 初始化、输出和正确性检查在外面。初步试过 n = 4，`C[0][0] = 50`，0.0016 ms；n = 64，`C[0][0] = 176800`，0.4099 ms。这两条不是正式样本。

截图里的 15 条我逐行抄进 [`experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv`](../experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv)。这是转录，不是程序导出。程序导出在同目录的 [`day04-local-program-export.csv`](../experiments/results/2026-10-26-day04-local/day04-local-program-export.csv)，数值不同，下面这张表不含它。截图转录那组的批量源码还没上传。[`templates/gpt/day04-benchmark-template.cpp`](../templates/gpt/day04-benchmark-template.cpp) 是 GPT 模板，不是我独立写的。和程序导出一起提交的源码在 [`code_history/day04/V07_user_ikj_program_export.cpp`](../code_history/day04/V07_user_ikj_program_export.cpp)。

| n | 样本 | 重复次数 | 单次中位数 (ms) | 单次均值 (ms) |
| ---: | ---: | ---: | ---: | ---: |
| 64 | 5 | 585 | 0.141690 | 0.142039 |
| 128 | 5 | 89 | 1.204964 | 1.396575 |
| 256 | 5 | 9 | 10.891122 | 10.859920 |

128 阶第 5 次单次 2.010587 ms，总时间 178.942200 ms，比同规模其他几次高。原值留着。原因暂留，以后再研究。

如果补测 128，旧样本留着，补测另记。然后继续看循环顺序和数据访问，再比分块。以后各版本的输入、正确性检查、计时范围和编译设置保持一致。

## 2026-10-26，Day 5

循环顺序这轮验收通过，只看今天的 30 条。学习用时未记录。环境和 Day 4 是同一套：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x。配置截图显示 Release | x64。更细的编译器版本号未记录。

我提交的双版本在 [`src/day05_ijk_ikj_benchmark.cpp`](../src/day05_ijk_ikj_benchmark.cpp)。`ijk` 和 `ikj` 同一次运行，奇数样本先 `ijk`，偶数样本先 `ikj`。计时包住 C 清零和乘法。输入初始化、校验和、逐元素比较在外面。笔记在 [`notes/day05-notes.md`](day05-notes.md)，任务记录在 [`notes/day05-record.md`](day05-record.md)，短实验记录在 [`experiments/reports/2026-10-26-day05-loop-order.md`](../experiments/reports/2026-10-26-day05-loop-order.md)。

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

这四天里，Day 1 是 30 分钟，Day 2 大约 20 分钟，Day 3 补记为 30 分钟，Day 4 补记为 2 小时，Day 5 的学习用时仍未记录。单元素程序编译运行输出 11。2×2 的原始输入和单位矩阵我都编译跑过。零矩阵输出是零矩阵，换回去和 Day 2 一致。O(n³) 以及“边长翻倍变成 8 倍”是我推出来的。Day 4 有截图转录和程序导出两份 15 条。截图转录那组的批量源码还没放进仓库。Day 5 的正式结果是今天的 30 条。比 18.x 更细的编译器版本号仍然未记录。
