# 2026-10-26 Day 5 今天的双版本（程序导出）

这是 2026-10-26 同一次运行里的 `ijk` 和 `ikj`。CSV 按验收包原样放进来，测量数字没有改。元数据在 [`metadata.md`](metadata.md)，统计在 [`summary.md`](summary.md)，读法在 [实验记录](../../reports/2026-10-26-day05-loop-order.md)。

| 文件 | 是什么 |
| --- | --- |
| [day05-loop-order-program-export.csv](day05-loop-order-program-export.csv) | 程序直接导出的 30 条。源码里的输出文件名是 `matmul_benchmark_day5_loop_order.csv`。全部 PASS，逐元素全部 YES。中位数只从这份算 |
| [visual-studio-release-x64.png](visual-studio-release-x64.png) | 验收包里的配置截图。截图显示 Visual Studio、Release、x64。它没有单独注明只属于哪一次运行，原图放在本目录，不复制到前一轮目录 |

前一轮只测 `ikj` 的 15 条不在这里，在 [`../2026-10-25-day05-ikj-only-prior/`](../2026-10-25-day05-ikj-only-prior/README.md)。那不是 Day 4 的 ijk 基线，也不和这 30 条做成同一轮算法对照。

我提交的双版本在 [`src/day05_ijk_ikj_benchmark.cpp`](../../../src/day05_ijk_ikj_benchmark.cpp)。[`templates/gpt/day05-loop-order-template.cpp`](../../../templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是这份 CSV 的程序。

CPU 型号、编译器具体版本、学习用时：未记录。
