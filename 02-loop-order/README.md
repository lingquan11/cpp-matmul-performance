# 第 2 章：循环顺序与缓存局部性

> 不改算法，只交换循环顺序，看访存方式会让耗时差多少。

## 本章收获
说清按行存储的二维数组在内存里怎么排，`ijk` 和 `ikj` 的内层循环分别怎样访问 B 和 C，并用同一口径的计时数据比较两者。

## 知识点地图
1. 行主序：`int B[256][256]` 中 `B[i][j]` 的地址是 `p + (i·256 + j)·4`
2. 缓存行与空间局部性：64 字节的缓存行放得下 16 个 `int`
3. `ijk` 的内层循环在 B 上跨行访问，`ikj` 的内层循环沿 B 和 C 的行连续访问
4. 累加之前必须把 C 清零
5. 只有耗时数据时，能证明什么、不能证明什么

## 讲义
| 讲义 | 内容 |
|---|---|
| [笔记 04：循环顺序](../journey/notes/04-loop-order.md) | `ijk` 与 `ikj` 的访问方式，第一次在同一程序里对比测量 |
| [笔记 05：内存布局与缓存](../journey/notes/05-memory-layout-and-cache.md) | 二维数组的地址计算、缓存行、连续访问与跨行访问 |
| 延伸阅读：Algorithmica [Spatial and Temporal Locality](https://en.algorithmica.org/hpc/external-memory/locality/) | 属于 C0-R01，在第 4 章开头读 |

## 动手实践

### 任务一：先预测
n = 256 时，`ijk` 和 `ikj` 的内层循环每次访问 `B[k][j]`，相邻两次的地址各差多少字节？先写下答案，再对照笔记 05。

### 任务二：在同一个程序里交替测两种顺序
1. 写 `matmul_ijk` 和 `matmul_ikj`，结果都和参考矩阵逐元素比较
2. n = 64、128、256，每种顺序 5 个样本；奇数样本先测 `ijk`，偶数样本先测 `ikj`
3. 取中位数，算 `ijk / ikj` 的比值

仓库里的程序：[results/src/loop-order.cpp](../results/src/loop-order.cpp)

思考：实测 `ikj` 更快，能不能单独证明它的缓存未命中更少？还需要什么证据？

### 任务三（支线，选做）：Performance Ninja 循环交换
做 [memory_bound/loop_interchange_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/memory_bound/loop_interchange_1)，先看实验页顶部的视频。对应战役 C0 的 S01。

## 成果展示
**实验 01：循环顺序 ijk vs ikj**

问题：只交换循环顺序（不改算法），按行存储的矩阵乘法能快多少？

结果表、结论和边界见 [实验 01](../results/experiments/01-loop-order/)，原始数据在 [data/loop-order.csv](../results/experiments/01-loop-order/data/loop-order.csv)，环境记录在 [metadata.md](../results/experiments/01-loop-order/metadata.md)。

## 与后续章节的关系
`ikj` 是第 3 章分块的出发点，也是第 4、5 章所有对比的基线。
