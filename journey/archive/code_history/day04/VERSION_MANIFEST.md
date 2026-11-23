# Day 4 代码版本目录

这些是 Day 4 的版本快照，不替换 `src/` 里现在的程序。

| 版本 | 文件 | 来源 | 主要变化 | 说明 |
|---|---|---|---|---|
| V01 | `V01_user_fixed_2x2.cpp` | 我在对话里粘贴的 | 固定 2×2，三重循环，输出整张矩阵 | 按我当时贴的内容留着。里面不一定每一行都是我自己写的 |
| V02 | `V02_gpt_fixed_capacity_manual_3x3.cpp` | GPT 模板 | 固定容量 `MAX_N`，有效规模 `n` 分开，手工填 3×3 | GPT 写的参考，不是我电脑上的源文件 |
| V03 | `V03_gpt_formula_initialized_fixed_n.cpp` | GPT 模板 | 用公式生成 A、B，计算和输出跟着 `n` | GPT 写的参考，不保证和我电脑上的文件一样 |
| V04 | `V04_gpt_runtime_n_bounds_check.cpp` | GPT 模板 | 运行时输入 n，检查 `1 <= n <= MAX_N` | GPT 写的参考。合法和非法规模我用截图对过，完整源码没交 |
| V05 | `V05_user_single_run_timing.cpp` | 我在对话里粘贴的 | `steady_clock` 单次计时，打印 `C[0][0]` | 按我当时贴的内容留着。里面不一定每一行都是我自己写的 |
| V06 | `V06_gpt_benchmark_template.cpp` | GPT 模板 | 64/128/256 批量重复，输出 CSV，检查 C00 和校验和 | GPT 模板。不是程序导出那份执行稿，也不是截图转录的执行稿 |
| V07 | `V07_user_ikj_program_export.cpp` | 和 Day 4 程序导出一起提交 | 只测 `ikj`，表头没有算法列和逐元素列 | 用户判定这份对应 [`day04-local-program-export.csv`](../../experiments/results/2026-10-26-day04-local/day04-local-program-export.csv)。历史路径曾误挂在 `code_history/day05/V01_yesterday_ikj_only.cpp`。文件内容没改。没有重跑，不写成已经逐行复现 |

GPT 模板是辅导时给的，不能标成我独立写的。截图转录那组的批量源码还没上传。V07 不代替 V06，两份哈希不同，都留着。
