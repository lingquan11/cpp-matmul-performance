# Day 5 代码版本历史

这是 Day 5 的快照：昨天只测 `ikj`，今天把 `ijk` 和 `ikj` 放在同一次运行里。来源看 [`VERSION_MANIFEST.md`](VERSION_MANIFEST.md)。

和 `src/` 里现在的文件一起留着。要改的话改 `src/`。没有盖掉 [`src/day04_matmul.cpp`](../../src/day04_matmul.cpp)，也没有改 `code_history/day04/`。

| 版本 | 文件 | 来源 | 和现行文件的关系 |
| --- | --- | --- | --- |
| V01 | `V01_yesterday_ikj_only.cpp` | 我此前提交的 | 只在本目录。昨天那 15 条 CSV 对的是这一份程序。按我交的内容留着，里面不一定每一行都是我自己写的 |
| V02 | `V02_today_ijk_ikj.cpp` | 我在 2026-10-26 提交的 | 和 [`src/day05_ijk_ikj_benchmark.cpp`](../../src/day05_ijk_ikj_benchmark.cpp) 是同一份复制。现行入口在 `src/` |

GPT 模板不在这个目录。它在 [`templates/gpt/day05-loop-order-template.cpp`](../../templates/gpt/day05-loop-order-template.cpp)，文件头写了「GPT 指导模板」。那份不是我独立写的，也不是我交上来的源码。
