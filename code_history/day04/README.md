# Day 4 代码版本历史

这是 Day 4 的演进快照：固定 2×2，容量和有效规模分开，公式初始化，运行时输入 n 并做边界检查，单次计时，再到 64/128/256 的批量基准模板。来源分类以 [`VERSION_MANIFEST.md`](VERSION_MANIFEST.md) 为准，这里不另改口径。

与 `src/` 现行文件并存，内容可能不同；`src/` 为现行入口，此处为版本历史。

| 版本 | 文件 | 来源 | 和现行文件的关系 |
| --- | --- | --- | --- |
| V01 | `V01_user_fixed_2x2.cpp` | 我在对话里实际粘贴 | 角色接近 [`src/day02_matrix.cpp`](../../src/day02_matrix.cpp)。不替换它 |
| V02 | `V02_gpt_fixed_capacity_manual_3x3.cpp` | GPT 指导模板 | 只在本目录。不是我独立写的 |
| V03 | `V03_gpt_formula_initialized_fixed_n.cpp` | GPT 指导模板 | 只在本目录。不是我独立写的 |
| V04 | `V04_gpt_runtime_n_bounds_check.cpp` | GPT 指导模板 | 只在本目录。不是我独立写的，也不当作我电脑上的完整源码 |
| V05 | `V05_user_single_run_timing.cpp` | 我在对话里实际粘贴 | 角色接近 [`src/day04_matmul.cpp`](../../src/day04_matmul.cpp)。两边都留着，哈希不同 |
| V06 | `V06_gpt_benchmark_template.cpp` | GPT 指导模板 | 演进副本。现行模板仍是 [`src/gpt-templates/day04_benchmark.cpp`](../../src/gpt-templates/day04_benchmark.cpp)，没有用 V06 覆盖。两份都不是我独立写的，也不保证和我电脑上实际执行的源码逐字一样 |

V02 到 V04、V06 不能标成我的独立产出。实际执行的完整基准测试源码和电脑直接导出的原始 CSV 还没上传。15 条正式样本仍是截图转录，在 [`experiments/results/2026-10-26_day04-local/`](../../experiments/results/2026-10-26_day04-local/README.md)。

仓库里没有完整的 Day 4 ZIP，所以这组文件只放在这里，没有追加进旧压缩包。
