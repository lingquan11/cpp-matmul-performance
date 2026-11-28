# 实验 01 元数据

- 日期：2026-10-26（学习记录中的 Day 5），`ijk` 和 `ikj` 在同一次运行中测量
- 环境：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.x，Release | x64，与 Day 4 相同（我确认）。配置截图只能看到 Release | x64，看不到更细的编译器版本和源文件名；CPU 截图在[归档](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/experiments/results/2026-10-26-day04-local/cpu-intel-core-ultra-7-255hx.png)
- 源码：[`results/src/loop-order.cpp`](../../src/loop-order.cpp)（原名 `src/day05-ijk-ikj-benchmark.cpp`，归档中的 [`code_history/day05/V02_today_ijk_ikj.cpp`](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/code_history/day05/V02_today_ijk_ikj.cpp) 是同一份）。归档中的 [GPT 模板](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/templates/gpt/day05-loop-order-template.cpp) 不是这份源码，也不是 CSV 的来源。源码里的泛型 lambda 需要 C++14 或更新
- 输入：`A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`
- 计时范围：C 清零和乘法在计时内；输入初始化、校验和、逐元素比较、输出在计时外
- 样本：n = 64、128、256，每种算法 5 个样本，共 30 条；奇数样本先 `ijk` 再 `ikj`，偶数样本反过来。每个样本内的重复次数：64 阶 `ijk` 613、`ikj` 3449；128 阶 75、570；256 阶 9、80
- 正确性：参考矩阵来自 `ijk` 试测；检查 `C[0][0]`、校验和与逐元素比较，30 条均为 PASS / YES。`C[0][0]`：64 阶 176800，128 阶 1406272，256 阶 11217536
- 原始数据：[`data/loop-order.csv`](data/loop-order.csv) 是程序直接导出的 CSV（程序写出名 `matmul_benchmark_day5_loop_order.csv`，入库前名为 `day05-loop-order-program-export.csv`），测量值没有改
- 统计：每组 5 个 `ms_per_multiply`（毫秒）取中位数；比值 = `ijk` 中位数 / `ikj` 中位数，保留两位小数。当时的汇总表在[归档](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/experiments/results/2026-10-26-day05-loop-order-local/summary.md)
- 备注：128 阶两组都有偏高的单次，保留原值，原因未查；`matmul_ikj` 累加行后的 TODO 注释和多余分号不影响结果；Day 4 的两组 15 条不计入
