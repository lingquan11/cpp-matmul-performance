# C0-P02 手写 AVX2 版 ikj

- **栏**：编程 | **计划用时**：2.5h | **先读**：C0-R03、R04（Algorithmica 的 SIMD Parallelism 与 Matrix Multiplication） | **依赖动手**：C0-H01（已完成）
- **一句话目标**：亲手写一次 SIMD 内层循环，知道 8 路 int32 理论上快 8 倍、实际差在哪里

## 材料（一手来源）
- Intel Intrinsics Guide（`_mm256_loadu_si256`、`_mm256_mullo_epi32`、`_mm256_add_epi32`、`_mm256_set1_epi32`、`_mm256_storeu_si256`）：https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

## 步骤
1. 【me】在 `results/src/large-matrix.hpp` 中写 `matmul_ikj_avx2`：A[i][k] 广播成向量，内层 j 循环每次处理 8 个 int，尾部用标量处理。
2. 【me】正确性：与 `matmul_ikj` 逐元素比较，包括 n 不是 8 的倍数（n = 1001），加进 `large-matrix-correctness.cpp`。
3. 【me】错误注入：把尾部处理删掉，确认 n = 1001 报 FAIL，再改回。
4. 【ai】CI 用 g++ 编译时加 `-mavx2`（只验证正确性，不计时）；计时框架的 `--algo ikj_avx2` 接上新 kernel，供 H04 测量。

## 机器验收（开工前写死，AI 不得修改）
- [ ] `matmul_ikj_avx2` 的正确性测试（含 n = 1001）PASS，错误注入那次 FAIL
- [ ] CI 为绿色

## 卡住时
- 超过 4h 仍不对：按砍线顺序第 3 条，H04 只做自动向量化对比，总结中写明。
