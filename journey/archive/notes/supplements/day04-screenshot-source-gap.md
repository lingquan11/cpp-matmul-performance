# Day 4 批量 benchmark 源码缺口说明

## 当前缺口

Day 4 有一组 15 条来自运行结果截图转录，文件是 [`experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv`](../../experiments/results/2026-10-26-day04-local/day04-local-screenshot-transcription.csv)。这组仍然没有用户当时实际运行的完整 benchmark `.cpp`。

同一天的另一份 15 条是程序导出，对应源码在 [`code_history/day04/V07_user_ikj_program_export.cpp`](../../code_history/day04/V07_user_ikj_program_export.cpp)，只含 `matmul_ikj`。它不能填上截图转录那组的源码缺口。我粘贴过的单次计时程序在 [`src/day04-matmul.cpp`](../../src/day04-matmul.cpp)，也不是那 15 条批量样本的执行稿。

## 为什么不能直接补成「用户实际源码」

1. 运行结果截图只能显示输出数值、字段或状态，无法恢复完整程序控制流、计时区间、重复次数计算、输入初始化、CSV 写入细节和所有正确性检查。
2. [`code_history/day04/V06_gpt_benchmark_template.cpp`](../../code_history/day04/V06_gpt_benchmark_template.cpp) 和 [`templates/gpt/day04-benchmark-template.cpp`](../../templates/gpt/day04-benchmark-template.cpp) 是 GPT 模板，不是用户电脑上实际执行的源码。
3. 如果依据截图重新编写一份程序，它只能是「重建版」或「GPT 模板」，不能被标记成「用户实际运行源码」。
4. 截图转录数据继续保留「截图转录」标签，不能把推测代码冒充原始材料。

## 正确处理方式

- 当前记录为：实际执行的完整 benchmark 源码未提供；这 15 条来自截图转录。
- 已入库的转录 CSV 和代码版本历史保留，并标明来源与局限。
- 若以后找到 Visual Studio 项目中的原始 `.cpp`，再追加补丁。若原文件已经丢失，这个缺口无法靠截图准确还原。
- 不为了让材料清单看起来完整而虚构源码，也不宣称 V06 就是实际执行版本。
