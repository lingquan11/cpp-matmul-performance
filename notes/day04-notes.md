# Day 4 学习笔记

今天我把固定规模的矩阵乘法改成可以输入阶数，并加上了边界检查。

`MAX_N` 是数组最多能放多大，`n` 是这次实际算多大。A 和 B 用公式填，再用三重循环算 C。`n = 0` 时循环进不去，但不符合这次的要求。`n = 257` 超过容量，不检查就可能越界。

计时用 `std::chrono::steady_clock`。计时包住 C 清零和矩阵乘法。A、B 初始化、校验和、正确性检查、输出都在外面。

校验和是把 C 的每个元素加起来，得到一个数。很多结果变了它能看出来。不同矩阵也可能加出同一个和，所以它单独证明不了每个元素都对。以后可以在计时结束后逐个元素对参考结果。

正式样本是 64、128、256，每个规模 5 次。截图转录 15 条都是 PASS。128 阶第 5 次明显偏高，原值留着。分析在 [`supplements/day04-day05-n128-high-samples.md`](supplements/day04-day05-n128-high-samples.md)，唯一根因仍未知。学习用时后来补记为 2 小时。

同一天还有一份程序直接导出的 15 条，也全部 PASS，耗时更快，重复次数不同。两份都在 `experiments/results/2026-10-26-day04-local/`，不合并。

这轮是辅导着做的。GPT 给的基准模板我放在 `templates/gpt/`，不是我独立写的。截图转录那组的实际完整 benchmark 源码还没上传；V06 不能冒充执行稿，说明在 [`supplements/day04-screenshot-source-gap.md`](supplements/day04-screenshot-source-gap.md)。和程序导出一起提交的源码在 `code_history/day04/V07_user_ikj_program_export.cpp`，只含 `ikj`。环境沿用 Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。Day 4–Day 7 同机。
