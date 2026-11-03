# 行跨度 LDA 报告（模板）

这是什么：把 day07 固定的 1024 字节行跨度改成运行时 `lda` 之后的报告模板。
谁写的：模板由 AI 起草。数字和结论等我在本机测完再填。
怎么用：把文件名里的 `YYYY-MM-DD` 换成我实际运行的日期。只抄 Windows 本机 CSV 里的数，并写上命令、环境和日期。
限制：现在没有实测。下面能填的位置都写「待测」。云端、Linux、虚拟机或 `tests/fixtures/layout-lda-fixture/` 里的数字不能填进来。

日期：待测

程序：[`templates/gpt/layout-lda.cpp`](../../templates/gpt/layout-lda.cpp)，不在 `src/`。`matmul_ijk_lda` 和 `matmul_ikj_lda` 还是 `TODO(零醛)`。`ijk_Bt` 没有建函数。

计时口径沿用仓库：计时含 C 清零和乘法，初始化、打印和正确性检查在外。初始化是 day07 的 `A=i+j+1`、`B=2*i+j+1`。n≤256 时 int 上界沿用 [`tests/README.md`](../../tests/README.md) 已写的 `256×511×766=100205056`，不是这次新测的。

判定用 10 个进程的中位数。进程中位数的算法依赖尚未合并的分支 `ideas/matmul-1002-i1-measurement-baseline` 里的 `experiments/reports/measurement-rules.md`（规则 R）。那份文件现在不在 main 上，本模板不抄一份进来，也不在本目录放一个同名链接。本实验自己的词是：n=256 的 ijk，`lda=272` 的中位数除以 `lda=256` 的中位数，≤0.90 且 10/10 都小于 1，记「行跨度显著影响」；0.97–1.03 记「无影响」；其余记「灰区」。这不是规则 R 的 5% 门槛。ikj 在各 LDA 上的相对极差是否 ≤3%，单独记，不把它写成 ijk 的结论。

C1：绑一个 P 核并加 `/high`。掩码待测，不在这里猜。

## n=256 ijk：lda=272 与 lda=256

待测

## ikj 在各 LDA 下的变化

待测

## 两种解释（并列，都还没被这次实验选定）

组冲突模型：day07 的 `int[256][256]` 一行是 1024 字节。`ijk` 内层按 k 跨行时，若这个跨度和 L1D 的组对得上，访问可能反复落到同一组。`lda` 改成 n、n+16 或 256 之后，机器码还是同一份，变的是跨度。这是解释。

4 KB 别名：有的处理器上，地址相差 4 KB 的整数倍时，load 和 store 会互相等。1024 字节本身不是 4 KB；隔若干行之后距离可以是 4 KB。这也是解释。

两条都先写在这里。哪一条更说得通，等本机 CSV 和 CPU-Z 的 L1D 容量、相联度齐了再写。硬件格子在 [`notes/dayNN-hw-facts.md`](../../notes/dayNN-hw-facts.md)，数值现在是待测。不另建一份硬件笔记。

## 结论

待零醛实测后填写。
