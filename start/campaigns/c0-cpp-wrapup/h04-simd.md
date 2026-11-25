# C0-H04 向量化报告与 SIMD 测量

- **栏**：动手（Muse 编译、测量和作图，解读由我写） | **计划用时**：1.5h | **先读**：C0-R02 | **依赖编程**：C0-P02 通过（未通过时只做第 1–2 步）
- **一句话目标**：搞清楚编译器有没有对 ikj 做向量化、用的是多宽的向量，以及手写 AVX2 能快多少

## 材料（一手来源）
- MSVC 自动向量化与 `/Qvec-report`：https://learn.microsoft.com/cpp/parallel/auto-parallelization-and-auto-vectorization ；https://learn.microsoft.com/cpp/build/reference/qvec-report-auto-vectorizer-reporting-level
- C5001/C5002 原因代码：https://learn.microsoft.com/cpp/error-messages/tool-errors/vectorizer-and-parallelizer-messages

## 步骤
1. Muse：运行 `results\scripts\vec-report.bat results\src\<包含 large-matrix.hpp 的 .cpp>`，得到 `/O2` 和 `/O2 /arch:AVX2` 两份报告原文与汇编，存入 `results/experiments/05-simd/`。
2. 【me】在报告中找到 ikj 内层循环那一行的 C5001 / C5002 和原因代码；在汇编中查找 `xmm` / `ymm`（`findstr /n "ymm" *.asm`），写下实际向量宽度。
3. Muse：测量 n ∈ {512, 1024, 2048} × {ikj `/O2`，ikj `/O2 /arch:AVX2`，`ikj_avx2`}，条件与 H03 相同；CSV、metadata、图。
4. 【me】写 `05-simd/README.md`（≤ 30 行）：三种配置的 GOPS 对比，附报告和汇编摘录；离 8 倍差多少、可能原因。

## 机器验收（开工前写死，AI 不得修改）
- [ ] 两种编译选项下的向量化报告原文入库，ikj 内层循环的 C5001 / C5002 可以查到
- [ ] 汇编摘录能看出 ikj 内层循环用的是 `xmm` 还是 `ymm`
- [ ] CSV、metadata、README 齐全，含三种配置 × 三个 n 的 GOPS 表，解读部分是 `[me]` 提交

## 卡住时
- `vec-report.bat` 跑不通：直接在 x64 Native Tools 中手敲两条 `cl` 命令，输出重定向到 txt。
