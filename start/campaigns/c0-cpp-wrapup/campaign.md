# 战役 C0：cpp-matmul 收尾 v1.0

- **周期**：2026-11-23 → 2026-12-06（截止日不延期）
- **预算**：每周 8–10h，合计 16–20h；必做线计划 14.5h
- **唯一交付物**：`v1.0` Release。`results/README.md` 用一张表回答"循环顺序、分块、SIMD 在不同规模下各带来什么"，并附 `results/summary.md`
- **检查点**：2026-11-29（复盘）、2026-12-06（收尾）
- **下一个战役**：[AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning) 的 C1 CUDA SGEMM（2026-12-07 起）

## 必做线
| ID | 任务 | 计划用时 | 验收要点 | 状态 |
|---|---|---:|---|---|
| T01 | 三区重构：start / journey / results | 1.5h | 见 [t01-restructure.md](t01-restructure.md) | 待我核对 |
| T02 | 硬件事实 + 纸面预测 | 2h | 见 [t02-hw-facts-and-prediction.md](t02-hw-facts-and-prediction.md) | 未开始 |
| T03 | 扩大规模实测：n = 256 → 2048 | 4h | 见 [t03-scale-sweep.md](t03-scale-sweep.md) | 未开始 |
| T04 | SIMD：自动向量化 vs AVX2 手写 | 4h | 见 [t04-simd.md](t04-simd.md) | 未开始 |
| T05 | 总结（≤ 1000 字） | 2h | 见 [t05-summary.md](t05-summary.md) | 未开始 |
| T06 | v1.0 收尾 | 1h | 见 [t06-release.md](t06-release.md) | 未开始 |

## 加分线（必做线全部通过后才碰）
| ID | 任务 | 计划用时 |
|---|---|---:|
| B01 | 用 Intel VTune 对 n = 2048 的 ikj 和分块各采一次缓存未命中，把 T03 的解释从"推断"升级为"测量" | 3h |
| B02 | `float` 版本的 ikj 和分块（为 CUDA SGEMM 铺路） | 2h |
| B03 | 分块 + 打包（packing），看能否超过 ikj | 3h |

## 砍线顺序
1. B03
2. B02
3. B01
4. （仍然超时）T04 缩小为只做自动向量化对比，不手写 intrinsics，在总结中写明

## 收尾定义
2026-12-06：必做线全部通过考官 → 打 `v1.0` → 进入 AI-Infra-Learning 的 C1。
有必做项没有完成：在总结的"未完成"一节写明原因，同样收尾。
