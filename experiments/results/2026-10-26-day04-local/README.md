# 2026-10-26 Day 4 本地

同一天有两份 15 条，都是 PASS，数值不同，不合并，也不互相覆盖。元数据在 [`metadata.md`](metadata.md)。

| 文件 | 是什么 |
| --- | --- |
| [day04-local-screenshot-transcription.csv](day04-local-screenshot-transcription.csv) | 运行截图逐行转录。重复次数 585 / 89 / 9。耗时在后来 Day 5 的 `ijk` 量级。不是程序导出 |
| [day04-local-program-export.csv](day04-local-program-export.csv) | 程序直接导出。重复次数 2208 / 552 / 84，耗时更快。用户确认这是 Day 4，不是 Day 5 |
| [cpu-intel-core-ultra-7-255hx.png](cpu-intel-core-ultra-7-255hx.png) | 任务管理器截图。处理器为 Intel(R) Core(TM) Ultra 7 255HX |

- 日期：2026-10-26
- 计时口径：含 C 清零与矩阵乘法；A/B 初始化、校验、输出在计时外
- 环境：用户确认 Windows，Visual Studio 2026 18.x，CPU 见上面的截图。配置截图显示 Release | x64，在 Day 5 结果目录，用户确认两天实验因素一致
- 128 阶偏高样本保留原值，原因暂留，以后再研究
- 截图转录那组的批量源码还没上传。GPT 模板不能当作执行稿
- 和程序导出一起提交的源码在 [`code_history/day04/V07_user_ikj_program_export.cpp`](../../../code_history/day04/V07_user_ikj_program_export.cpp)，只含 `matmul_ikj`。原先误放在 `code_history/day05/`
- 我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](../../../src/day04_matmul.cpp)
- 统计见 [`summary.md`](summary.md)。缺口见 [`missing-materials.md`](missing-materials.md)
