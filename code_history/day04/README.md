# Day 4 代码版本历史

这是 Day 4 的演进快照：固定 2×2，容量和有效规模分开，公式初始化，运行时输入 n 并做边界检查，单次计时，再到 64/128/256 的批量基准模板。来源看 [`VERSION_MANIFEST.md`](VERSION_MANIFEST.md)。

和 `src/` 里现在的文件一起留着，内容可能不一样。要改的话改 `src/`。

| 版本 | 文件 | 来源 | 和现行文件的关系 |
| --- | --- | --- | --- |
| V01 | `V01_user_fixed_2x2.cpp` | 我在对话里粘贴的 | 和 [`src/day02_matrix.cpp`](../../src/day02_matrix.cpp) 差不多。不替换它 |
| V02 | `V02_gpt_fixed_capacity_manual_3x3.cpp` | GPT 模板 | 只在本目录。不是我独立写的 |
| V03 | `V03_gpt_formula_initialized_fixed_n.cpp` | GPT 模板 | 只在本目录。不是我独立写的 |
| V04 | `V04_gpt_runtime_n_bounds_check.cpp` | GPT 模板 | 只在本目录。不是我独立写的，也不当作我电脑上的完整源码 |
| V05 | `V05_user_single_run_timing.cpp` | 我在对话里粘贴的 | 和 [`src/day04_matmul.cpp`](../../src/day04_matmul.cpp) 差不多。两边都留着，哈希不同 |
| V06 | `V06_gpt_benchmark_template.cpp` | GPT 模板 | 和 [`templates/gpt/day04-benchmark-template.cpp`](../../templates/gpt/day04-benchmark-template.cpp) 是两份，没有用这份盖掉那份。都不是我独立写的，也不保证和我电脑上跑的一样 |

V02、V03、V04、V06 是 GPT 模板，不是我独立写的。我实际跑的完整基准源码和原始 CSV 还没上传。15 条正式样本是截图转录，在 [`experiments/results/2026-10-26-day04-local/`](../../experiments/results/2026-10-26-day04-local/README.md)。

仓库里没有完整的 Day 4 ZIP，所以这些文件只放在这个目录。
