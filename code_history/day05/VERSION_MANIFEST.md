# Day 5 代码版本目录

这些是 Day 5 的快照，不替换 `src/` 里现在的程序，也不改 Day 4 的文件。

| 版本 | 文件 | 来源 | 主要变化 | 说明 |
|---|---|---|---|---|
| V01 | `V01_yesterday_ikj_only.cpp` | 我此前提交的 | 只测 `ikj`，输出不含算法列和逐元素列 | 按我交的内容留着。里面不一定每一行都是我自己写的。对应 [`day05-ikj-only-program-export.csv`](../../experiments/results/2026-10-25-day05-ikj-only-prior/day05-ikj-only-program-export.csv) 的 15 条，全部 PASS |
| V02 | `V02_today_ijk_ikj.cpp` | 我在 2026-10-26 提交的 | `ijk` 与 `ikj` 同一次运行，奇数样本和偶数样本交换先后 | 和 [`src/day05_ijk_ikj_benchmark.cpp`](../../src/day05_ijk_ikj_benchmark.cpp) 是同一份复制。对应 [`day05-loop-order-program-export.csv`](../../experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv) 的 30 条，全部 PASS，逐元素全部 YES |

GPT 模板不在本表里。文件是 [`templates/gpt/day05-loop-order-template.cpp`](../../templates/gpt/day05-loop-order-template.cpp)。它是辅导时给的参考，文件头写了「GPT 指导模板」，不能标成我的独立产出，也不代替我提交的 V02。

## 材料

- 前一轮 15 条在 [`experiments/results/2026-10-25-day05-ikj-only-prior/`](../../experiments/results/2026-10-25-day05-ikj-only-prior/README.md)，今天 30 条在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../../experiments/results/2026-10-26-day05-loop-order-local/README.md)。都是原样复制，测量数字没有改。
- 前一轮 15 条和今天 30 条可以并列留着。日期和程序结构不同，不作同轮算法对照，也不把前一轮当成 Day 4 的 ijk 基线。
- 验收包原件在 [`archive/2026-10-26-day05-final-acceptance/`](../../archive/2026-10-26-day05-final-acceptance/README.md)。
- 配置截图显示 Visual Studio Release | x64。CPU 型号、编译器具体版本、学习用时：未记录。
- V01 和 V02 的 `matmul_ikj` 里，累加那一行后面还留着 TODO 注释和多出来的分号。可以以后清，这次计算不受影响。V02 的泛型 lambda 需要 C++14 或更新的标准。这是源码的要求，不是我记下来的编译器版本。
