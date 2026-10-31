# 我放下的实验结果

一组实验一个目录，目录名用 `YYYY-MM-DD-<experiment>-<environment>`。日期或环境没有记下来的，在该目录的 `metadata.md` 里写明，不把猜测写进测量值。里面放 csv、图和短说明。试测我留在本地，放进仓库的是我决定采用的那一次。不把同一份 CSV 复制到多份报告目录，也不覆盖旧数据。

| 目录 | 内容 |
| --- | --- |
| [2026-10-26-day04-local/](2026-10-26-day04-local/) | Day 4。截图转录和程序导出各 15 条，分文件存放，不合并。读法在 [实验记录](../reports/2026-10-26-day04-local.md) |
| [2026-10-26-day05-loop-order-local/](2026-10-26-day05-loop-order-local/) | Day 5 今天的双版本，30 条程序导出。读法在 [实验记录](../reports/2026-10-26-day05-loop-order.md) |
| [2026-10-27-day07-blocking-local/](2026-10-27-day07-blocking-local/) | Day 7 分块本地实验，75 条程序导出与边界截图。读法在 [实验记录](../reports/2026-10-27-day07-blocking.md) |

Day 8 没有结果目录：那天没有真实硬件计数器采集，也没有正式 benchmark CSV。对话里的带宽、IPC、MLP、延迟、Core Bound、Memory Bound、Pipeline Slots 和 A/B/C/D 数字是教学示例，只留在 [`notes/day08-notes.md`](../../notes/day08-notes.md) 和 [Day 8 方法报告](../reports/day08-performance-analysis.md)，不进本目录。

以后我自己测稳、决定公开的计时结果，再按日期放在这个目录里。现在没有可跑的云端基准脚本。编译检查不在这里跑计时。Day 4、Day 5、Day 7 的计时复现：未验证。
