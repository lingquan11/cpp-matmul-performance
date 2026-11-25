# 路线图

这个仓库只有一个战役 C0。它是 [AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning) 路线的前置：先在 CPU 上建立访存、分块和 SIMD 的直觉，再迁移到 GPU。

| ID | 战役 | 窗口 | 预算 | 唯一交付物 | 状态 |
|---|---|---|---:|---|---|
| C0 | cpp-matmul 收尾 v1.0 | 2026-11-23 → 2026-12-06 | 16–20h | `v1.0` Release：循环顺序 / 分块 / SIMD 在不同规模下的结论表 + 总结 | 进行中 |

- **主线**：主教材 Sergey Slotin, [*Algorithms for Modern Hardware*](https://en.algorithmica.org/hpc/)（Algorithmica）第 8–11 章 → 大矩阵 kernel 与 AVX2（编程）→ 规模扫描、向量化报告、总结与 v1.0（动手）
- **支线**（选做）：[Performance Ninja](https://github.com/dendibakh/perf-ninja) 的循环交换、分块、向量化三个实验

C0 之后：转到 AI-Infra-Learning 的 C1 CUDA SGEMM（2026-12-07 起）。本仓库的分块和 SIMD 结论，就是 C1 中共享内存分块和 `float4` 向量化的对照组。
