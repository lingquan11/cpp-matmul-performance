# 我的实验

分析写在 [`reports/`](reports/README.md)，原始 CSV、图和环境记录放在 [`results/`](results/README.md)。当前做到哪以仓库根的 [`PROJECT_STATE.md`](../PROJECT_STATE.md) 为准。

| 实验 | 报告 | 原始数据 |
| --- | --- | --- |
| 2026-10-26，Day 4 本地：截图转录和程序导出各 15 条 | [2026-10-26-day04-local.md](reports/2026-10-26-day04-local.md) | [2026-10-26-day04-local/](results/2026-10-26-day04-local/) |
| 2026-10-26，Day 5 今天的 `ijk` / `ikj`，30 条 | [2026-10-26-day05-loop-order.md](reports/2026-10-26-day05-loop-order.md) | [2026-10-26-day05-loop-order-local/](results/2026-10-26-day05-loop-order-local/) |
| 2026-10-27，Day 7 分块，75 条 | [2026-10-27-day07-blocking.md](reports/2026-10-27-day07-blocking.md) | [2026-10-27-day07-blocking-local/](results/2026-10-27-day07-blocking-local/) |
| Day 8 性能诊断方法（日期未记录），无实测 | [day08-performance-analysis.md](reports/day08-performance-analysis.md) | 没有。文中数字是教学示例 |
| 阶段 A：2×2 算法实现与正确性 | [stage-a-algorithm-implementation.md](reports/stage-a-algorithm-implementation.md) | 这次没有单独的测量目录 |

## 从归档 CSV 再打一遍中位数

[`summarize_medians.py`](summarize_medians.py) 只读两份已经入库的程序导出：

- Day 5：[`results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv`](results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv)
- Day 7：[`results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv`](results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv)

每组 5 个 `ms_per_multiply`，中位数取排序后的第 3 个。终端里打出的是 Day 5 中位数和 `ijk / ikj`，以及 Day 7 报告里的中位数、`ijk / ikj`、分块相对 `ikj` 的变化。比值写到两位小数。它不改 CSV，也不运行计时程序。

在仓库根目录：

```bash
python3 experiments/summarize_medians.py
```

需要 Python 3，用标准库，不用额外安装。打出来的中位数应对上 [Day 5 报告](reports/2026-10-26-day05-loop-order.md) 和 [Day 7 报告](reports/2026-10-27-day07-blocking.md)。对不上就停下来查，不要改报告里的旧数字去迁就新输出。计时复现：未验证。


2026-11-01：[/O2 与 /Od、分块复测](reports/2026-11-01-compiler-blocking-windows.md) · [原始数据与环境](results/2026-11-01-compiler-blocking-windows/metadata.md)。
