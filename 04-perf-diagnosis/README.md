# 第 4 章：性能诊断与规模扫描

> 先用硬件事实做纸面预测，再把规模扩大到 n = 2048 去测，用数据检验预测。

本章属于战役 C0（2026-11-23 → 2026-12-06），任务状态以 [campaign.md](../start/campaigns/c0-cpp-wrapup/campaign.md) 和首页的学习进度为准。笔记 07 是 C0 之前学的（记录用时 5h），C0 里本章计划 9.5h。

## 本章收获
学完应该能回答（自测问题来自 campaign.md 的阅读表）：
- 为什么用"读写了多少个缓存块"而不是运算次数来衡量？
- 带宽受限和延迟受限怎样区分？为什么步长是 2 的幂时可能突然变慢？
- n ≤ 256 时分块为什么不快？从多大的 n 开始，分块可能比 `ikj` 快？什么样的结果会证明预测错了？

## 知识点地图
1. 存储层次与外部存储模型，cache-oblivious 算法和手写分块的关系
2. 带宽与延迟、缓存行、预取、相联度
3. 性能模型 T ≈ I / (IPC × F)、Top-down Pipeline Slots、2×2 因子实验
4. 工作集与缓存容量：三个矩阵共 3·n²·4 B，一个块组共 3·b²·4 B，各落在哪一级缓存
5. 先提交可证伪的预测，再测量

## 讲义
| 讲义 | 内容 |
|---|---|
| [笔记 07：CPU 性能诊断](../journey/notes/07-cpu-performance-diagnosis.md) | 性能模型、未命中次数与未命中率、带宽与延迟、MLP、Top-down、实验设计 |
| C0-R01 Algorithmica 第 8 章 [External Memory](https://en.algorithmica.org/hpc/external-memory/) | 存储层次、外部存储模型、cache-oblivious 算法、局部性 |
| C0-R02 Algorithmica 第 9 章 [RAM & CPU Caches](https://en.algorithmica.org/hpc/cpu-cache/) | 带宽、延迟、缓存行、预取、相联度 |

每篇读哪几节，见 campaign.md 的阅读表。

## 动手实践
按 campaign.md 的推荐顺序：

| 顺序 | 任务 | 目标 | 任务卡 |
|---|---|---|---|
| 0 | C0-H01 环境 | Coreinfo、MSVC 命令行、Python + matplotlib 可用 | campaign.md 动手表 |
| 1 | C0-R01、R02 阅读 | 读完再做预测 | 见上方讲义 |
| 2 | C0-H02 硬件事实 + 纸面预测 | 测量之前，预测分块从多大的 n 开始可能比 `ikj` 快 | [h02](../start/campaigns/c0-cpp-wrapup/h02-hw-facts-and-prediction.md) |
| 3 | C0-P01 大矩阵版 ikj 与分块 | 把 kernel 搬到能跑 n = 2048 的版本，通过正确性测试和错误注入 | [p01](../start/campaigns/c0-cpp-wrapup/p01-large-matrix.md) |
| 4 | C0-H03 规模扫描 | 在 n = 256 → 2048 上测量 `ikj` 和分块，与预测逐条对照 | [h03](../start/campaigns/c0-cpp-wrapup/h03-scale-sweep.md) |

支线（选做）：Performance Ninja 的循环交换、分块实验，见第 2、3 章。

## 成果展示
实验 04 规模扫描在 C0 中产出，完成后在 `results/experiments/04-scale-sweep/`：硬件事实表、纸面预测、GOPS 表和图，以及预测与实测的逐条对照。

## 与后续章节的关系
P01 写好的大矩阵 kernel 和计时框架，第 5 章直接拿来加 SIMD。
