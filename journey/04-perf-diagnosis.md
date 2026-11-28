# 第 4 章：性能诊断与规模扫描（进行中）

> 先用硬件事实做纸面预测，再把规模扩大到 n = 2048 去测，用数据检验预测。

本章在战役 C0（2026-11-23 → 2026-12-06）中完成；任务状态以 [campaign.md](../start/campaigns/c0-cpp-wrapup/campaign.md) 为准。

## 要回答的问题
- 为什么用“读写了多少个缓存块”而不是运算次数来衡量？
- 带宽受限和延迟受限怎样区分？为什么步长是 2 的幂时可能突然变慢？
- n ≤ 256 时分块为什么不快？从多大的 n 开始分块可能比 `ikj` 快？什么样的结果会证明预测错了？

## 要点
- 存储层次与外部存储模型；cache-oblivious 算法和手写分块的关系
- 带宽与延迟、缓存行、预取、相联度
- 性能模型 T ≈ I / (IPC × F)、Top-down Pipeline Slots、2×2 因子实验
- 工作集与缓存容量：三个矩阵共 3·n²·4 B，一组块共 3·b²·4 B，各落在哪一级缓存
- 先提交可证伪的预测，再测量

## 讲义
- [我的笔记：CPU 性能诊断](notes/cpu-performance-diagnosis.md)：Day 8 讲性能模型、未命中次数与未命中率、带宽与延迟、MLP、Top-down、实验设计
- Algorithmica 第 8 章 [External Memory](https://en.algorithmica.org/hpc/external-memory/)（R01）：存储层次、外部存储模型、cache-oblivious 算法、局部性
- Algorithmica 第 9 章 [RAM & CPU Caches](https://en.algorithmica.org/hpc/cpu-cache/)（R02）：带宽、延迟、缓存行、预取、相联度

## 步骤
1. 环境已就绪（H01）：Coreinfo、MSVC 命令行、Python + matplotlib，见 [environment.md](../results/hw/environment.md)
2. 读 R01、R02（每篇读哪几节见 campaign.md 的阅读表）
3. [H02 硬件事实 + 纸面预测](../start/campaigns/c0-cpp-wrapup/h02-hw-facts-and-prediction.md)：**测量之前**写下分块从多大的 n 开始可能比 `ikj` 快
4. [P01 大矩阵版 ikj 与分块](../start/campaigns/c0-cpp-wrapup/p01-large-matrix.md)：把 kernel 改成能跑 n = 2048 的版本，通过正确性测试和错误注入
5. [H03 规模扫描](../start/campaigns/c0-cpp-wrapup/h03-scale-sweep.md)：在 n = 256 → 2048 上测 `ikj` 和分块，与预测逐条对照

选做：Performance Ninja 的循环交换、分块实验，见第 2、3 章。

## 产出
`results/experiments/04-scale-sweep/`：硬件事实表、纸面预测、GOPS 表和图、预测与实测的逐条对照。P01 的大矩阵 kernel 和计时框架在[第 5 章](05-simd.md)直接拿来加 SIMD。
