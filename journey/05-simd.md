# 第 5 章：SIMD 与向量化（未开始）

> 在第 4 章的大矩阵 kernel 上亲手写一次 SIMD 内层循环，弄清编译器的自动向量化做了什么，以及 8 路 int32 理论上快 8 倍、实际差在哪里。

本章在战役 C0 中完成；任务状态以 [campaign.md](../start/campaigns/c0-cpp-wrapup/campaign.md) 为准。

## 要回答的问题
- 一条 AVX2 指令一次处理几个 int32？未对齐加载有什么代价？
- 编译器在什么条件下才会自动向量化？
- 转置、向量化、寄存器复用的微内核、分块各解决了什么瓶颈？这能不能解释第 3 章的分块没有变快？

## 要点
- 向量寄存器与 intrinsics：`_mm256_loadu_si256`、`_mm256_mullo_epi32`、`_mm256_add_epi32`、`_mm256_set1_epi32`、`_mm256_storeu_si256`
- 把 `A[i][k]` 广播成向量，内层 j 循环一次处理 8 个 int，尾部用标量处理
- MSVC 自动向量化报告：`/Qvec-report`、C5001 / C5002 与原因代码
- 从汇编里分辨 `xmm` 和 `ymm`
- 理论加速比与实测之间的差距

## 讲义
- Algorithmica 第 10 章 [SIMD Parallelism](https://en.algorithmica.org/hpc/simd/)（R03）：intrinsics 与向量类型、数据搬运、自动向量化
- Algorithmica 第 11 章 [Matrix Multiplication](https://en.algorithmica.org/hpc/algorithms/matmul/)（R04）：转置、向量化、微内核、分块
- [Intel Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)：写 P02 时查阅

## 步骤
1. 读 R03、R04
2. [P02 手写 AVX2 版 ikj](../start/campaigns/c0-cpp-wrapup/p02-avx2.md)：写 `matmul_ikj_avx2`，n = 1001 这类有尾部的情况也要通过正确性测试和错误注入
3. [H04 向量化报告与 SIMD 测量](../start/campaigns/c0-cpp-wrapup/h04-simd.md)：编译器有没有向量化 `ikj`、用多宽的向量，手写 AVX2 能快多少

选做：Performance Ninja [core_bound/vectorization_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/core_bound/vectorization_1)，读完 R03 后做。

## 产出
`results/experiments/05-simd/`：两种编译选项下的向量化报告原文和汇编摘录，三种配置 × 三个规模的 GOPS 表和解读。结论和第 4 章一起进入[第 6 章](06-summary.md)的总结。
