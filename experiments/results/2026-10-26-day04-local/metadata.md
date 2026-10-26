# 元数据：2026-10-26 Day 4 本地

- 编号 / 日期：Day 4。日期是 2026-10-26。用户确认程序导出就是这一天的，不再把它记成「昨天」或 2026-10-25。
- 目标：64、128、256 各 5 次正式计时。本目录有两份材料，不合并。
- 源码路径：
  - 截图转录没有对应的已上传批量源码。我粘贴过的单次计时程序在 [`src/day04_matmul.cpp`](../../../src/day04_matmul.cpp)，不是这批批量样本。[`templates/gpt/day04-benchmark-template.cpp`](../../../templates/gpt/day04-benchmark-template.cpp) 和 [`code_history/day04/V06_gpt_benchmark_template.cpp`](../../../code_history/day04/V06_gpt_benchmark_template.cpp) 是 GPT 模板，不能当作执行稿。
  - 程序导出和 [`code_history/day04/V07_user_ikj_program_export.cpp`](../../../code_history/day04/V07_user_ikj_program_export.cpp) 一起提交。该文件只调用 `matmul_ikj`，写出的表头和文件名 `matmul_benchmark_raw.csv` 与这份 CSV 一致。重复次数由试测决定，源码里没有写死耗时。没有在本环境重跑，所以不把「表头一致」写成已经逐行复现。此文件原先误放在 `code_history/day05/V01_yesterday_ikj_only.cpp`，内容没有改。
- 编译器 / 参数：用户口述 IDE 是 Visual Studio 2026，版本 18.x。没有更细的版本号，MSVC 工具集号未记录。配置截图在 [`../2026-10-26-day05-loop-order-local/visual-studio-release-x64.png`](../2026-10-26-day05-loop-order-local/visual-studio-release-x64.png)，可见 Visual Studio、Release、x64。用户确认 Day 4 与 Day 5 的实验因素保持一致。计时范围沿用已有记录：含 C 清零和矩阵乘法，输入初始化、正确性检查、输出在计时外。
- CPU / OS：用户确认操作系统是 Windows。CPU 是 Intel Core Ultra 7 255HX。截图 [`cpu-intel-core-ultra-7-255hx.png`](cpu-intel-core-ultra-7-255hx.png) 是任务管理器性能页，处理器一栏为 Intel(R) Core(TM) Ultra 7 255HX。
- 矩阵生成：`A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`。两份 CSV 的 `C[0][0]` 和校验和一致：64 阶 176800 / 1781137408，128 阶 1406272 / 57131663360，256 阶 11217536 / 1830369099776。
- 计时范围：见上。Day 3 的方案把清零放在计时外，和这两天分开读。
- 样本数 / 重复 / 单位：两份都是 15 条，每个规模 5 次，单位毫秒。截图转录的重复次数是 64 阶 585、128 阶 89、256 阶 9。程序导出的重复次数是 64 阶 2208、128 阶 552、256 阶 84。
- 正确性检查：两份都是 `C[0][0]` 和校验和，15 条都是 PASS。都没有逐元素列。
- 原始数据来源：
  - [`day04-local-screenshot-transcription.csv`](day04-local-screenshot-transcription.csv)：运行截图逐行转录。任务记录把这组写成 i-j-k。耗时在后来 Day 5 的 `ijk` 量级。
  - [`day04-local-program-export.csv`](day04-local-program-export.csv)：程序直接导出。字节与用户确认的 Day 4 导出一致。验收包曾经把它放在 `04_实验数据/昨天_单算法测试/matmul_benchmark_raw.csv`，那个叫法作废。导出耗时更快，和只含 `matmul_ikj` 的 V07 放在一起。
- 统计方法：截图转录的中位数和均值只在 [`summary.md`](summary.md)，不把程序导出的数算进去。程序导出不在这里新算一套中位数，避免和转录或 Day 5 排成一张对照表。
- 解释与限制：两份材料数值不同，禁止合并或互相覆盖。例如 n = 64 的第 1 次，转录是 0.139568 ms，导出是 0.020481 ms。重复次数也不同。128 阶偏高样本保留原值，原因暂留，以后再研究。截图转录里是第 5 次 2.010587 ms。学习用时用户补记为 2 小时。截图转录那组的批量源码仍然未上传。
