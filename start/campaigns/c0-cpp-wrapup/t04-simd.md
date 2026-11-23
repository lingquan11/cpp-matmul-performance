# T04 SIMD：自动向量化 vs AVX2 手写

- **线**：必做 | **计划用时**：4h | **前置**：T03（复用其 kernel 和框架）
- **一句话目标**：搞清楚编译器有没有对 ikj 做向量化、用的是多宽的向量，以及手写 AVX2 能快多少

## 材料（一手来源）
- MSVC 自动向量化：https://learn.microsoft.com/cpp/parallel/auto-parallelization-and-auto-vectorization
- `/Qvec-report`：https://learn.microsoft.com/cpp/build/reference/qvec-report-auto-vectorizer-reporting-level
- `/arch:AVX2`：https://learn.microsoft.com/cpp/build/reference/arch-x64
- C5001/C5002 原因代码：https://learn.microsoft.com/cpp/error-messages/tool-errors/vectorizer-and-parallelizer-messages
- Intel Intrinsics Guide（`_mm256_loadu_si256`、`_mm256_mullo_epi32`、`_mm256_add_epi32`、`_mm256_set1_epi32`）：https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html

## 步骤
1. 【ai】让 `results/scripts/vec-report.bat` 指向 `large-matrix.cpp`（目前指向 `blocking.cpp`），输出到 `results/experiments/05-simd/`。
2. 【me】用 `/O2 /Qvec-report:2` 编译 T03 的 ikj，记录内层循环的 C5001 或 C5002 以及原因代码。
3. 【me】再用 `/O2 /arch:AVX2 /Qvec-report:2` 编译，用 `/FAs` 生成汇编，在 ikj 内层循环中查找 `xmm` 和 `ymm`（可以用 `results/scripts/scan-asm-keywords.py` 辅助），确定实际向量宽度。
4. 【me】在 `results/src/large-matrix.cpp` 中手写 `matmul_ikj_avx2`：内层 j 循环每次处理 8 个 int，尾部用标量处理。
5. 【me】正确性：与 ikj 逐元素比较，包括 n 不是 8 的倍数（n = 1001）。加进 `large-matrix-correctness.cpp`。
6. 【me】测量：n ∈ {512, 1024, 2048} × {ikj `/O2`, ikj `/O2 /arch:AVX2`, ikj_avx2}，测量条件与 T03 相同。
7. 【me】按模板写 `05-simd/README.md`（≤ 30 行）：三种配置的 GOPS 对比，附向量化报告和汇编证据的摘录。

## 所有权
- `[me]`：`matmul_ikj_avx2`、汇编阅读结论、README 中的分析
- `[ai]`：编译脚本、框架接入、作图

## 机器验收（开工前写死，AI 不得修改）
- [ ] 两种编译选项下的向量化报告原文（txt）入库，ikj 内层循环那一行的 C5001/C5002 可以查到
- [ ] 汇编摘录中能看出 ikj 内层循环用的是 `xmm` 还是 `ymm`
- [ ] `matmul_ikj_avx2` 的正确性测试（含 n = 1001）PASS，CI 为绿色
- [ ] CSV、metadata 和 README 齐全，包含三种配置 × 三个 n 的 GOPS 表

## 闭卷要点
- 默认 `/O2` 在 x64 上用哪一级 SIMD？为什么要加 `/arch:AVX2` 才可能用上 256 位？
- 为什么 ikj 中 j 循环最容易向量化？A[i][k] 在向量化之后扮演什么角色？
- 8 路 int32 理论上快 8 倍，实测多少？差距来自哪里（访存瓶颈？`mullo` 的延迟和吞吐？）
- n = 2048 时 SIMD 的收益比 n = 512 时大还是小？为什么？
- 和 CUDA 中 `float4` 向量化访存有什么共同点？（为 C1 铺垫）

## 卡住时
- 超过 6h，或手写 intrinsics 一直不对：按砍线规则第 4 条，只交付第 2、3、6 步，手写部分写"未完成"。
