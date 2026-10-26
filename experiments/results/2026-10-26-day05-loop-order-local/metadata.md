# 元数据：2026-10-26 Day 5 今天的双版本

- 编号 / 日期：Day 5。这组是 2026-10-26 的同一次运行，`ijk` 和 `ikj` 放在一起测。
- 目标：在同一程序、同一轮样本里比较 `i-j-k` 和 `i-k-j`。
- 源码路径：[`src/day05-ijk-ikj-benchmark.cpp`](../../../src/day05-ijk-ikj-benchmark.cpp)。[`code_history/day05/V02_today_ijk_ikj.cpp`](../../../code_history/day05/V02_today_ijk_ikj.cpp) 是同一份复制。程序用 `ofstream` 写出的文件名是 `matmul_benchmark_day5_loop_order.csv`。GPT 模板在 [`templates/gpt/day05-loop-order-template.cpp`](../../../templates/gpt/day05-loop-order-template.cpp)，不是这份提交，也不是这份 CSV 的来源。
- 编译器 / 参数：用户确认与 Day 4 的实验因素一致。IDE 是 Visual Studio 2026 18.x，没有更细的版本号。配置截图可见 Release | x64。源码里的泛型 lambda 需要 C++14 或更新标准，这是语言要求，不是记下来的工具集号。计时范围沿用已有记录，见下一条。
- CPU / OS：用户确认与 Day 4 相同：Windows，Intel Core Ultra 7 255HX。CPU 截图在 [`../2026-10-26-day04-local/cpu-intel-core-ultra-7-255hx.png`](../2026-10-26-day04-local/cpu-intel-core-ultra-7-255hx.png)，这里不复制。配置截图上还可见调试下拉「本地 Windows 调试器」，打开的文件名是「源1.cpp」。截图没有显示仓库里的源文件名，也没有显示比 18.x 更细的编译器版本号。
- 矩阵生成：`A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`。写在提交的源码里。
- 计时范围：C 清零和矩阵乘法在计时内。输入初始化、校验和、逐元素比较、输出在计时外。
- 样本数 / 重复 / 单位：n = 64、128、256，每种算法 5 次，共 30 条。奇数样本先 `ijk` 再 `ikj`，偶数样本反过来。重复次数：64 阶 `ijk` 613、`ikj` 3449；128 阶 `ijk` 75、`ikj` 570；256 阶 `ijk` 9、`ikj` 80。`ms_per_multiply` 的单位是毫秒。
- 正确性检查：参考矩阵来自 `ijk` 试测。检查 `C[0][0]`、校验和和逐元素比较。30 条状态都是 PASS，逐元素比较都是 YES。同一规模的 `C[0][0]`：64 阶 176800，128 阶 1406272，256 阶 11217536。
- 原始数据来源：[`day05-loop-order-program-export.csv`](day05-loop-order-program-export.csv) 是程序直接导出的 CSV，按验收包原样放入，测量数字没有改。验收包里的文件名是 `matmul_benchmark_day5_loop_order.csv`。
- 统计方法：每组 5 个 `ms_per_multiply` 取中位数。比值是 `ijk` 中位数除以 `ikj` 中位数，写到两位小数。均值、最小、最大也只从这 30 条来。表在 [`summary.md`](summary.md)。
- 解释与限制：今天这一轮里 `ikj` 的中位数低于 `ijk`。比值只描述这次程序、这次规模、这次运行。128 阶两组都有偏高的单次，原值留着，原因暂留，以后再研究。`matmul_ikj` 累加那一行后面留着 TODO 注释和多出来的分号，这次计算不受影响。学习用时未记录。Day 4 的两份 15 条不放进这些中位数。
