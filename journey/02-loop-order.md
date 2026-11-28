# 第 2 章：循环顺序与缓存局部性

> 不改算法，只交换循环顺序，看访存方式会让耗时差多少。

## 要点
- 行主序：`int B[256][256]` 中 `B[i][j]` 的地址是 `p + (i·256 + j)·4`
- 缓存行与空间局部性：一条 64 字节的缓存行放得下 16 个 `int`
- `ijk` 的内层循环在 B 上跨行访问；`ikj` 的内层循环沿 B 和 C 的行连续访问
- `ikj` 是累加写法，开始前必须把 C 清零
- 只有耗时数据时，能证明什么、不能证明什么

## 讲义
- [我的笔记：循环顺序与缓存](notes/loop-order-and-cache.md)：Day 5 讲 `ijk` 与 `ikj` 的访问方式，第一次在同一程序里对比测量；Day 6 讲二维数组的地址计算、缓存行、连续访问与跨行访问
- 延伸：Algorithmica [Spatial and Temporal Locality](https://en.algorithmica.org/hpc/external-memory/locality/)（战役 C0 的 R01，第 4 章开头读）

## 练习
1. **先预测**：n = 256 时，`ijk` 和 `ikj` 的内层循环相邻两次访问 `B[k][j]`，地址各差多少字节？写下答案，再对照笔记中的 Day 6
2. **在同一个程序里交替测两种顺序**：写 `matmul_ijk` 和 `matmul_ikj`，结果都和参考矩阵逐元素比较；n = 64、128、256，每种顺序 5 个样本，奇数样本先测 `ijk`、偶数样本先测 `ikj`；取中位数，算 `ijk / ikj`。参考程序：[results/src/loop-order.cpp](../results/src/loop-order.cpp)
3. **思考**：实测 `ikj` 更快，能不能单独证明它的缓存未命中更少？还需要什么证据？
4. **选做**：Performance Ninja [memory_bound/loop_interchange_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/memory_bound/loop_interchange_1)（先看实验页顶部的视频）

## 成果：实验 01
n = 64–256 时，`ikj` 比 `ijk` 快 6.85–9.38 倍（中位数），规模越大差距越大。结果表、边界和原始数据见 [实验 01](../results/experiments/01-loop-order/)。

**下一章**：[第 3 章](03-blocking.md)，以 `ikj` 为起点做分块；它也是第 4、5 章所有对比的基线。
