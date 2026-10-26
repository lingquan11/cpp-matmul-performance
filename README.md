# cpp-matmul-performance

[![小规模基准](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml/badge.svg)](https://github.com/lingquan11/cpp-matmul-performance/actions/workflows/bench.yml)

I'm a first-year electronic information student at Southeast University. I am measuring a C++ i-j-k matrix multiply from n = 64 to 1024, because I want to see how data reuse shows up before I look at NPUs.

我是东南大学电子信息大一。矩阵乘法是 NPU 和 AI 芯片上最重的一块计算，我想先在 CPU 上把最朴素的 i-j-k 跑明白：规模变大以后，耗时会不会还按运算次数走。

i-j-k 基线我已经做完了。`src/day02_matrix.cpp` 是固定 2×2，输出 `11 10` 和 `19 30`。2026-10-26 我在云端 VM 上把 n 从 64 测到 1024，每个规模 5 个正式样本。计时口径我还在用这次扫描的做法。下一步我打算换一种循环顺序，再试分块，然后看脉动阵列和 SCALE-Sim。每天的笔记在 [`notes/`](notes/)，眼下的安排写在 [`PROJECT_STATE.md`](PROJECT_STATE.md)。

n=1024 我测到 0.569162 GFLOPS，n=64 是 6.929414，1024 大约只有 64 的 8.2%。我想弄清楚这 8.2% 是从哪一档规模开始掉的。数字来自 2026-10-26 04:05 UTC 那次云端 VM，只跟这次实验里的其他规模比。口径和环境我写在 [实验记录](docs/report.md)。

![i-j-k 三重循环在不同矩阵规模下的耗时与 GFLOPS](results/2026-10-26_ijk-scale/perf.png)

| 规模 n | 单次耗时中位数 (ms) | GFLOPS |
| ---: | ---: | ---: |
| 64 | 0.075661 | 6.929414 |
| 128 | 1.285246 | 3.263424 |
| 256 | 13.434294 | 2.497670 |
| 512 | 104.810185 | 2.561158 |
| 1024 | 3773.059512 | 0.569162 |

每一次的原始样本在 [`results/2026-10-26_ijk-scale/raw.csv`](results/2026-10-26_ijk-scale/raw.csv)。

我在本机复现用这一条：

```bash
python3 benchmarks/run_bench.py
```

需要 `g++` 和 Python 3。画图还要 `matplotlib`，没有的话这条命令会试着装。默认规模是 64、128、256、512、1024，每个规模 5 个正式样本，结果写到 `results/<日期>_matmul/`。脚本会把 `benchmarks/kernels/` 里的实现画在同一张图上，现在里面是 i-j-k。我之后把循环重排或分块放进这个目录，还是跑同一条命令，放法在 [`benchmarks/README.md`](benchmarks/README.md)。

推到 `main` 时，上面的徽章会编译并跑一次 n=64。

程序在 `src/`（Day 1 算一个元素，Day 2 是 2×2）。计时在 `benchmarks/`。CSV 和图在 `results/`。这次扫描的记录是 [`docs/report.md`](docs/report.md)，阶段 A 的报告是 [`reports/stage-a-algorithm-implementation.md`](reports/stage-a-algorithm-implementation.md)。
