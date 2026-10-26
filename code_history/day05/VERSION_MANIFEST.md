# Day 5 代码版本目录

这些快照不替换 `src/` 里现在的程序，也不改 Day 4 的其他文件。

| 版本 | 文件 | 来源 | 主要变化 | 说明 |
|---|---|---|---|---|
| V02 | `V02_today_ijk_ikj.cpp` | 我在 2026-10-26 提交的 | `ijk` 与 `ikj` 同一次运行，奇数样本和偶数样本交换先后 | 和 [`src/day05_ijk_ikj_benchmark.cpp`](../../src/day05_ijk_ikj_benchmark.cpp) 是同一份复制。对应 [`day05-loop-order-program-export.csv`](../../experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv) 的 30 条，全部 PASS，逐元素全部 YES |

原先的 V01（`V01_yesterday_ikj_only.cpp`）已移到 [`code_history/day04/V07_user_ikj_program_export.cpp`](../day04/V07_user_ikj_program_export.cpp)。用户判定它对应 Day 4 程序导出。历史路径曾误挂在 day05。没有另写一份源码。

GPT 模板不在本表里。文件是 [`templates/gpt/day05-loop-order-template.cpp`](../../templates/gpt/day05-loop-order-template.cpp)。它是辅导时给的参考，文件头写了「GPT 指导模板」，不能标成我的独立产出，也不代替我提交的 V02。

## 材料

- Day 5 的正式数据只有今天 30 条，在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../../experiments/results/2026-10-26-day05-loop-order-local/README.md)。原样复制，测量数字没有改。
- 曾被本目录称作「昨天 15 条」的 CSV 已改记为 Day 4 程序导出，在 [`experiments/results/2026-10-26-day04-local/day04-local-program-export.csv`](../../experiments/results/2026-10-26-day04-local/day04-local-program-export.csv)。
- 验收包原件在 [`archive/2026-10-26-day05-final-acceptance/`](../../archive/2026-10-26-day05-final-acceptance/README.md)。包内旧目录名还写着「昨天」，以用户这次的判定为准。
- 环境与 Day 4 一致：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x。配置截图显示 Release | x64。比 18.x 更细的编译器版本号、学习用时：未记录。
- V02 的 `matmul_ikj` 里，累加那一行后面还留着 TODO 注释和多出来的分号。可以以后清，这次计算不受影响。V02 的泛型 lambda 需要 C++14 或更新的标准。这是源码的要求，不是我记下来的编译器版本。
