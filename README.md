# cpp-matmul-performance

[![小规模基准](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml/badge.svg)](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml)

**English.** Undergraduate project at Southeast University (electronic information, first year). It implements a C++ i-j-k matrix multiplication, checks it on small cases, and measures runtime from n = 64 to 1024 on a cloud VM. The numbers are a baseline for how data reuse shows up in NPU workloads. The badge tracks the benchmark workflow, which compiles the code and runs n = 64.

矩阵乘法是 NPU / AI 芯片上最主要的计算。这里用 C++ 把 i-j-k 三重循环做成可复现的实验，看矩阵规模变大以后，耗时怎样偏离运算次数，从而理解数据复用。作者是东南大学电子信息大一。

## 进度

| 状态 | 内容 |
| --- | --- |
| 已完成 | i-j-k 基线：`src/day02_matrix.cpp` 的 2×2 结果为 `11 10` / `19 30`；2026-10-26 在云端 VM 上完成 n=64～1024 的规模扫描 |
| 进行中 | 性能测量：计时口径、每个规模 5 次样本，以及上面这次扫描 |
| 计划中 | 循环重排；分块；脉动阵列与 SCALE-Sim |

每日学习记录在 [`notes/`](notes/)，当前计划在 [`PROJECT_STATE.md`](PROJECT_STATE.md)。

## 目前的结果

目前已完成 i-j-k 基线。n=1024 的 GFLOPS 是 **0.569162**，n=64 为 6.929414，前者约为后者的 8.2%。数字来自 2026-10-26 04:05 UTC 的云端 VM，用于这次实验内部的比较。口径、环境和分析在 [实验记录](docs/report.md)。

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

需要 `g++` 和 Python 3。画图需要 `matplotlib`；本机还没有时，这条命令会尝试安装。默认规模 64、128、256、512、1024，每个规模 5 个正式样本，输出到 `results/<日期>_matmul/`。

脚本编译 `benchmarks/kernels/` 中的每个实现，并画在同一张图上。当前内核是 i-j-k。循环重排或分块写好后，按 [`benchmarks/README.md`](benchmarks/README.md) 放入该目录，再运行同一条命令。

推送到 `main` 时，徽章对应的工作流会编译并运行一次 n=64。

## 目录

```
src/            学习程序（Day 1 单元素，Day 2 的 2×2）
benchmarks/     计时驱动；kernels/ 中的实现进入性能图
results/        实测 CSV 和图
docs/report.md  规模扫描的实验记录
reports/        阶段 A 报告
notes/          每日学习记录
```

阶段 A 报告：[`reports/stage-a-algorithm-implementation.md`](reports/stage-a-algorithm-implementation.md)。
