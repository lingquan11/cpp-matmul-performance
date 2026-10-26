# 2026-10-26 Day 4 本地基准（截图转录）

这是 Day 4 目标③的正式样本记录。我把运行截图逐行转录成 [`raw.csv`](raw.csv)，**不是**从电脑直接读出的原始 CSV；原始 CSV 和实际执行的完整基准源码都还没上传。

- 规模：64 / 128 / 256，各 5 次正式样本，截图中均为 PASS
- 计时口径：含 C 清零与矩阵乘法；A/B 初始化、校验、输出在计时外
- 编译器、CPU、是否 Release/x64：**未记录**
- 128 阶第 5 次明显偏高，原始值保留
- GPT 基准模板在 [`src/gpt-templates/day04_benchmark.cpp`](../../../src/gpt-templates/day04_benchmark.cpp)，**不能**当作我实际执行的源码
- 我实际粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](../../../src/day04_matmul.cpp)
- 缺口清单见 [`missing-materials.md`](missing-materials.md)；统计说明见 [`summary.md`](summary.md)

公开仓里曾经放过的云端规模扫描展示已按我的要求撤下。这组只是 Day 4 本地截图转录。
