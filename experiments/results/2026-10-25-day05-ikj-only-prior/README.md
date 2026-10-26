# Day 5 前一轮：只测 ikj（程序导出）

这是双版本对比之前的另一轮。验收包把它叫做相对 2026-10-26 的「昨天」。CSV 里没有日历日期，目录上的 2026-10-25 只表示这个说法，不是文件里的时间戳。测量数字没有改。元数据在 [`metadata.md`](metadata.md)。

| 文件 | 是什么 |
| --- | --- |
| [day05-ikj-only-program-export.csv](day05-ikj-only-program-export.csv) | 程序直接导出的 15 条，全部 PASS。程序里的输出文件名是 `matmul_benchmark_raw.csv`。验收包路径是 `04_实验数据/昨天_单算法测试/matmul_benchmark_raw.csv` |

对应源码只在 [`code_history/day05/V01_yesterday_ikj_only.cpp`](../../../code_history/day05/V01_yesterday_ikj_only.cpp)。今天的 30 条在 [`../2026-10-26-day05-loop-order-local/`](../2026-10-26-day05-loop-order-local/README.md)。

不要把这 15 条标成 Day 4 的 ijk 基线。Day 4 的截图转录在 [`../2026-10-26-day04-local/`](../2026-10-26-day04-local/README.md)，那份不是程序导出。
