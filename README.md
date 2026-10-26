# cpp-matmul-performance

[![小规模基准](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml/badge.svg)](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml)

**English.** Early-stage undergraduate project at Southeast University (electronic information, first year). It checks a naive C++ i-j-k matrix multiplication and measures that one kernel from n = 64 to 1024 on a cloud VM. The aim is to see, with reproducible numbers, why data movement matters for NPUs. Loop reordering, tiling, and systolic arrays are planned, not implemented. The badge is the real status of the 「小规模基准」 GitHub Action (compile and run n = 64). It is not a score.

矩阵乘法是 NPU / AI 芯片上最主要的计算。这个仓库从朴素三重循环做起，用可复现的计时看规模变大以后耗时为什么不跟运算次数走，从而理解数据复用为什么重要。作者是东南大学电子信息大一，项目还在早期。

## 进度

| 状态 | 内容 |
| --- | --- |
| 已完成 | 固定 2×2 的 i-j-k（`src/day02_matrix.cpp`），原始结果 `11 10` / `19 30`。2026-10-26 在云端 VM 上测了 n=64～1024 |
| 进行中 | 性能测量。计时口径、5 次样本和这一次规模扫描已经进仓库。循环重排还没有代码 |
| 计划中 | 分块；脉动阵列与 SCALE-Sim。做完才会出现在对比图里 |

学习过程中的每日记录在 [`notes/`](notes/)，不在首页展开。学习计划原文在 [`PROJECT_STATE.md`](PROJECT_STATE.md)：仓库里的这次扫描不能代替作者自己把 Day 4 的步骤做完。

## 目前的结果

只有一种实现。n=1024 的 GFLOPS 是 **0.569162**，约为 n=64（6.929414）的 8.2%。数字来自 2026-10-26 04:05 UTC 那次云端 VM 运行，只适合看这次实验内部的相对关系。口径、环境和能说的分析在 [实验记录](docs/report.md)。

![i-j-k 三重循环在不同矩阵规模下的耗时与 GFLOPS](results/2026-10-26_ijk-scale/perf.png)

| 规模 n | 单次耗时中位数 (ms) | GFLOPS |
| ---: | ---: | ---: |
| 64 | 0.075661 | 6.929414 |
| 128 | 1.285246 | 3.263424 |
| 256 | 13.434294 | 2.497670 |
| 512 | 104.810185 | 2.561158 |
| 1024 | 3773.059512 | 0.569162 |

原始样本在 [`results/2026-10-26_ijk-scale/raw.csv`](results/2026-10-26_ijk-scale/raw.csv)。

## 复现

```bash
python3 benchmarks/run_bench.py
```

需要 `g++` 和 Python 3。画图需要 `matplotlib`；没有安装时这条命令会尝试安装。默认规模 64、128、256、512、1024，每个规模 5 个正式样本，输出到 `results/<日期>_matmul/`。

脚本会自动编译 `benchmarks/kernels/` 里的每一个实现，并画进同一张图。现在这个目录里只有 i-j-k。自己写了循环重排或分块之后，把新文件放进去再跑同一条命令即可，步骤写在 [`benchmarks/README.md`](benchmarks/README.md)。

推送到 `main` 时，上面的徽章对应的工作流会编译并跑一次 n=64，只检查能否通过，不更新图。

## 目录

```
src/          已有的学习程序（Day 1 单元素，Day 2 的 2×2）
benchmarks/   计时驱动。kernels/ 放要上图的实现
results/      实测 CSV 和图
docs/report.md  给这次规模扫描写的实验记录
reports/      阶段 A 的学习报告
notes/        每日学习记录
```

阶段 A 报告：[`reports/stage-a-algorithm-implementation.md`](reports/stage-a-algorithm-implementation.md)。
