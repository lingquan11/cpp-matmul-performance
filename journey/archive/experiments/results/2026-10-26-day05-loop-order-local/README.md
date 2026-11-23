# 2026-10-26 Day 5 今天的双版本（程序导出）

这是 2026-10-26 同一次运行里的 `ijk` 和 `ikj`。CSV 按导出原样放进来，测量数字没有改。验收 ZIP 原件不再入库。元数据在 [`metadata.md`](metadata.md)，统计在 [`summary.md`](summary.md)，读法在 [实验记录](../../reports/2026-10-26-day05-loop-order.md)。

| 文件 | 是什么 |
| --- | --- |
| [day05-loop-order-program-export.csv](day05-loop-order-program-export.csv) | 程序直接导出的 30 条。源码里的输出文件名是 `matmul_benchmark_day5_loop_order.csv`。全部 PASS，逐元素全部 YES。中位数只从这份算 |
| [visual-studio-release-x64.png](visual-studio-release-x64.png) | 配置截图。可见 Visual Studio、Release、x64。用户确认 Day 5 与 Day 4 是同一套环境 |

Day 4 的截图转录和程序导出不在这里，在 [`../2026-10-26-day04-local/`](../2026-10-26-day04-local/README.md)。那两份 15 条不计入这 30 条。

我提交的双版本在 [`src/day05-ijk-ikj-benchmark.cpp`](../../../src/day05-ijk-ikj-benchmark.cpp)。[`templates/gpt/day05-loop-order-template.cpp`](../../../templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是这份 CSV 的程序。

环境与 Day 4 相同：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x。CPU 截图放在 Day 4 目录。学习用时未记录。比 18.x 更细的编译器版本号未记录。
