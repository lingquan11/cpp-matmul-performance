# 元数据：2026-10-26 Day 4 本地正式样本

- 编号 / 日期：Day 4 目标③。学习记录上的日期是 2026-10-26。
- 目标：64、128、256 各 5 次正式计时，看规模变大以后单次耗时怎么变。
- 源码路径：实际执行的完整基准源码未上传。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](../../../src/day04_matmul.cpp)，它不是这 15 条正式样本的那份批量程序。[`templates/gpt/day04-benchmark-template.cpp`](../../../templates/gpt/day04-benchmark-template.cpp) 是 GPT 模板，不能当作这次执行的源码。版本快照在 [`code_history/day04/`](../../../code_history/day04/README.md)。
- 编译器 / 参数：未记录。
- CPU / OS：未记录。是不是 Release/x64：未记录。
- 矩阵生成：任务记录写的是 `A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`。执行用的完整源码没上传，公式以任务记录为准。
- 计时范围：含 C 清零和矩阵乘法。A、B 初始化、校验、输出在计时外。
- 样本数 / 重复 / 单位：每个规模 5 次，共 15 条。重复次数 64 阶 585、128 阶 89、256 阶 9。耗时列是 `ms_per_multiply`，单位毫秒，由该次 `total_ms` 除以重复次数得到。
- 正确性检查：`C[0][0]` 和校验和。15 条状态都是 PASS。没有逐元素比较。
- 原始数据来源：[`day04-local-screenshot-transcription.csv`](day04-local-screenshot-transcription.csv) 是运行截图逐行转录，不是程序直接导出的 CSV。电脑直接导出的原始 CSV 未上传。
- 统计方法：每个规模 5 个 `ms_per_multiply` 取中位数和算术平均。说明在 [`summary.md`](summary.md)。
- 解释与限制：128 阶第 5 次明显偏高，原值留着，原因未定。只有 5 次。PASS 不能证明每个元素都对过。编译器、CPU、操作系统、构建配置、学习用时都未记录。不要把这份 ijk 转录和 Day 5 的 `ikj` 前一轮当成同一轮对照。
