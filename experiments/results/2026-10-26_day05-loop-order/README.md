# 2026-10-26 Day 5 循环顺序

这里有两份 CSV，都是原样复制，测量数字没有改。读法在 [实验记录](../../reports/2026-10-26-day05-loop-order.md)。

| 文件 | 是什么 |
| --- | --- |
| [raw-today-loop-order.csv](raw-today-loop-order.csv) | 今天的双版本测试。30 条，`ijk` 和 `ikj`，全部 PASS，逐元素全部 YES。中位数比值只从这份算 |
| [raw-yesterday-ikj-only.csv](raw-yesterday-ikj-only.csv) | 昨天的单算法测试。15 条，全部 PASS。没有算法列，也没有逐元素列 |
| [visual_studio_release_x64.png](visual_studio_release_x64.png) | 配置截图。截图显示 Visual Studio Release \| x64 |

昨天和今天不是同一天跑的，程序结构也不一样。可以并列留在这个目录里，不当成同一轮的算法对照。

我提交的双版本源码在 [`src/day05_ijk_ikj_benchmark.cpp`](../../../src/day05_ijk_ikj_benchmark.cpp)。昨天的单算法快照在 [`code_history/day05/`](../../../code_history/day05/README.md)。[`src/gpt-templates/day05_loop_order_comparison.cpp`](../../../src/gpt-templates/day05_loop_order_comparison.cpp) 是 GPT 模板，不是我独立写的。

CPU 型号、编译器具体版本、学习用时：未记录。
