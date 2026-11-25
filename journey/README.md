# 学习过程

| 时间 | 里程碑 | 证据 |
|---|---|---|
| 2026-10-20 | 手算并用程序算出矩阵乘法的一个元素 | [notes/01](notes/01-matmul-by-hand.md) |
| 2026-10-23 | 正确性检查（零矩阵、单位矩阵）与测量计划 | [notes/02](notes/02-correctness-and-measurement.md) |
| 2026-10-26 | 第一次计时：`steady_clock`、校验和、重复取样 | [notes/03](notes/03-timing-and-checksums.md) |
| 2026-10-26 | 循环顺序：`ikj` 比 `ijk` 快 6.85–9.38 倍 | [results/01](../results/experiments/01-loop-order/) |
| 2026-10-27 | 二维数组内存布局、缓存行与空间局部性 | [notes/05](notes/05-memory-layout-and-cache.md) |
| 2026-10-27 | 分块实现：n ≤ 256 时没有加速 | [results/02](../results/experiments/02-blocking/) |
| 日期未记录 | CPU 性能诊断：缓存、IPC/CPI、Top-down、实验设计 | [notes/07](notes/07-cpu-performance-diagnosis.md) |
| 2026-11-01 | 编译选项复测：`/O2` 下分块仍慢于 `ikj` | [results/03](../results/experiments/03-compiler-flags/) |
| 2026-11-23 | 仓库重构为 start / journey / results 三区，开始收尾战役 C0 | [日志](hands-on-log.md) |
| 2026-11-25 | C0-T01 通过，首页补上当前位置与下一步的入口 | [验收](https://github.com/lingquan11/cpp-matmul-performance/actions/runs/45610239444) |
| 2026-11-25 | 改为三栏学习：阅读长文 / 编程（GPT）/ 动手（Muse）；删除未用脚手架，旧归档移到 tag | [战役 C0](../start/campaigns/c0-cpp-wrapup/campaign.md) |

- 日志：[编程](coding-log.md)（GPT）· [动手与阅读](hands-on-log.md)（Muse）
- 笔记（按主题）：[notes/](notes/)
- 重构前的原始材料（逐日记录、对话、旧计划、未完成的脚手架）：tag [archive-2026-11-23](https://github.com/lingquan11/cpp-matmul-performance/tree/archive-2026-11-23/journey/archive) 中的 `journey/archive/`，不再维护
