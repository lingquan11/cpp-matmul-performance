# SIMD 对照报告（模板）

这是什么：默认编译（脚本里的 V1）和 `/arch:AVX2`（V2）的计时对照模板，加上一份编译器检查的位置。
谁写的：模板由 AI 起草。数字、关键字计数和「我的判断」等我在本机看完再填。
怎么用：把文件名里的 `YYYY-MM-DD` 换成实际运行的日期。编译检查用 [`scripts/vec-report.bat`](../../scripts/vec-report.bat)，只做一次，行跨度实验复用它。计时用 [`scripts/run-simd-factor.bat`](../../scripts/run-simd-factor.bat)。
限制：现在没有实测，也没有提交 `.asm`。云端、Linux、虚拟机和 `tests/fixtures/asm-keyword-fixture/` 的计数不能填进来。

日期：待测

程序：[`templates/gpt/simd-factor.cpp`](../../templates/gpt/simd-factor.cpp)，不在 `src/`。`matmul_ikj_simd` 和 `matmul_blocked_simd` 还是 `TODO(零醛)`。day07 源码没有改。

## 编译器检查

待测。把 `vec-report-v1.txt`、`vec-report-v2.txt` 的结论抄过来，并写上编译器版本。

MSVC 19.51 是否打印 C5001/C5002，以本机 txt 为准。Microsoft Learn 记载了这两个消息号，但本仓库的云端环境没有 cl，不能把文档写成已经在 19.51 上见过。

若报告显示未向量化，只记录在 [`notes/dayNN-asm-reading.md`](../../notes/dayNN-asm-reading.md)，不改代码去迎合编译器。

关键字表最后一列留空，等我填。计数本身不是「已经向量化」或「没有向量化」。

## 计时

待测。

顺序是 V1、V2、V2、V1，算法是 `ikj`、`blocked_16`、`blocked_64`，n 是 256 和 1024。每一格脚本现在只跑 1 个进程。规则 R 要 10 个进程才写快或慢。规则全文在尚未合并的分支 `ideas/matmul-1002-i1-measurement-baseline` 的 `experiments/reports/measurement-rules.md`，本模板不复制。要不要把每一格扩成 10 个进程，我再定。

C1 掩码：待测。

n=1024 不能沿用 day07 的 int 初始化，乘加上界会超出 32 位有符号整数。公式还没定，脚手架不另造一个。

V0（`-DNO_VEC` / `#pragma loop(no_vector)`）是扩展，不在这四周里。2×2 交互项等我自己做。

## 我预先不写的话

AVX2 不一定比默认编译快，更没有「一定快 8 倍」。V1 和 V2 接近，本身就是可以写下来的结果。

## 结论

待零醛实测后填写。
