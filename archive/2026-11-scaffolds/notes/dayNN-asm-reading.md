# 汇编阅读笔记（日程序号未定）

这不是新的学习日。`notes/` 里已经有到 day08 的笔记。序号还没定，所以文件名叫 `dayNN`，不提前占用 day09。

待零醛填写。

脚手架在 [`templates/gpt/simd-factor.cpp`](../templates/gpt/simd-factor.cpp)，不在 `src/`。编译检查用 [`scripts/vec-report.bat`](../scripts/vec-report.bat) 和 [`scripts/scan-asm-keywords.py`](../scripts/scan-asm-keywords.py)，和行跨度实验共用这一道闸门，不另写一份。day07 的源码不改。

若 vec-report 显示未向量化，我只把原文记在表里，不改代码去迎合编译器。V1 和 V2 接近，本身也是发现。我不会预先写成 AVX2 一定快 8 倍。

## 怎么读

这些是指令和寄存器的公开说明，不是我这台电脑已经编出来的结果。本机的 `.asm` 还没有生成。

- `xmm` 是 128 位寄存器，装得下 4 个 int32。`ymm` 是 256 位寄存器，装得下 8 个 int32。出处：Intel 64 and IA-32 Architectures Software Developer's Manual, Volume 1，关于 XMM/YMM 的章节。
- `pmulld` 是 SSE4.1 指令，两组 packed signed dword 相乘，各留下低 32 位。出处：同一套手册 Volume 2 的 PMULLD。Intel Intrinsics Guide 里 `_mm_mullo_epi32` 标的是 SSE4.1。
- `vpmulld` 是它的 VEX 形式。用 `ymm` 做 8 路 int32 乘法时，Intrinsics Guide 把 `_mm256_mullo_epi32` 标成 AVX2。出处：手册 Volume 2 的 VPMULLD，以及 [Intrinsics Guide](https://www.intel.com/content/www/us/en/docs/intrinsics-guide/index.html)。
- MSVC 的 x64 默认就有 SSE2，`/arch:SSE2` 在 x64 上不单独存在；`/arch:AVX2` 才打开 AVX2。出处：[ /arch (x64) ](https://learn.microsoft.com/en-us/cpp/build/reference/arch-x64)。所以脚本里的 V1 是「不加 /arch」，V2 是加上 `/arch:AVX2`。这是文档，不是我已经核对过的 19.51 列表。
- `/Qvec-report:2` 的消息号，Microsoft Learn 写的是 C5001（loop vectorized）和 C5002（loop not vectorized because of reason）。出处：[Vectorizer and parallelizer messages](https://learn.microsoft.com/en-us/cpp/error-messages/tool-errors/vectorizer-and-parallelizer-messages)。我这台机器还没跑过 cl，19.51 到底打不打印这两条，以本机 txt 为准。

关键字计数不是结论。`ymm` 出现也不说明整段循环已经向量化。

## 空表

| 源码行号 | vec-report 结论 | 关键字计数 | 我的判断 | 依据 |
| --- | --- | --- | --- | --- |
|  |  |  |  |  |

函数名、编译器版本、命令和日期等我跑完 `scripts/vec-report.bat` 再补。不要把夹具 `tests/fixtures/asm-keyword-fixture/` 里的计数填进这张表。
