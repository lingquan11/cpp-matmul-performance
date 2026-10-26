# Day 5 代码版本历史

这里只留今天的双版本快照。来源看 [`VERSION_MANIFEST.md`](VERSION_MANIFEST.md)。

曾放在本目录的 `V01_yesterday_ikj_only.cpp` 已移到 [`code_history/day04/V07_user_ikj_program_export.cpp`](../day04/V07_user_ikj_program_export.cpp)。用户确认它对应的 CSV 是 Day 4 程序导出，不是 Day 5。文件内容没有改。

`code_history/day05/` 是这一天的演变快照。现行成品在 `src/`，文件名用连字符。两处并存是正常的，哈希不同不合成一份。要改的话改 `src/`。没有盖掉 [`src/day04-matmul.cpp`](../../src/day04-matmul.cpp)。

| 版本 | 文件 | 来源 | 和现行文件的关系 |
| --- | --- | --- | --- |
| V02 | `V02_today_ijk_ikj.cpp` | 我在 2026-10-26 提交的 | 和 [`src/day05-ijk-ikj-benchmark.cpp`](../../src/day05-ijk-ikj-benchmark.cpp) 是同一份复制。现行入口在 `src/` |

GPT 模板不在这个目录。它在 [`templates/gpt/day05-loop-order-template.cpp`](../../templates/gpt/day05-loop-order-template.cpp)，文件头写了「GPT 指导模板」。那份不是我独立写的，也不是我交上来的源码。
