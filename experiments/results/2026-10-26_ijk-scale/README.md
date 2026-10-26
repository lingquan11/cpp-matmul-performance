# 我这次扫的 i-j-k

我测的是 i-j-k 三重循环，和 `src/day02_matrix.cpp` 相同，上界换成参数 n。Day 1 那个程序只算 C[0][0]。测量时源文件我记成 `benchmarks/ijk_kernel.cpp`，现在放在 `benchmarks/kernels/ijk.cpp`。这个目录里的 CSV 和图就是那次编译跑出来的。我怎么读，写在 [实验记录](../../reports/2026-10-26-ijk-scale.md)。

时间是 2026-10-26T04:05:29Z。系统是 Ubuntu 24.04.5 LTS，Linux 6.12.94+，KVM 云端 VM。`lscpu` 型号 Intel(R) Xeon(R) Processor（family 6，model 207，stepping 2），4 vCPU。L1d 192 KiB，L1i 128 KiB，L2 8 MiB，L3 320 MiB。MemTotal 16387280 kB。编译器 g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0。编译命令是 `g++ -O2 -std=c++17 -Wall -Wextra -Werror -o build/matmul_bench benchmarks/matmul_bench.cpp benchmarks/ijk_kernel.cpp`。图用的字体是 WenQuanYi Micro Hei（/usr/share/fonts/truetype/wqy/wqy-microhei.ttc）。

我计的是结果元素清零和 i-j-k 乘加。构造输入、校准和正确性检查不在正式样本里。一次调用比我设的最短样本时长短时，就在同一样本里连着重复，单次耗时 = 样本总耗时 / 重复次数。GFLOPS = 2 * n^3 / 单次耗时秒 / 1e9，每次乘加我算成 1 次乘法和 1 次加法。内核是 32 位整数，和我现有的实现一样。这些数字用来在这次实验内部比，不是浮点峰值。

原始数据在 `raw.csv`，汇总在 `summary.csv`，图是 `perf.png`，环境全文在 `environment.txt`。

| 规模 n | 样本内重复 | 正式样本 | 单次耗时中位数 (ms) | 单次耗时最小到最大 (ms) | GFLOPS（中位数耗时） | 相对上一档耗时倍数 |
| --- | ---: | ---: | ---: | --- | ---: | ---: |
| 64 | 1322 | 5 | 0.075661 | 0.075447 到 0.077330 | 6.929414 | 无 |
| 128 | 79 | 5 | 1.285246 | 1.283210 到 1.333168 | 3.263424 | 16.986855 |
| 256 | 8 | 5 | 13.434294 | 13.413632 到 13.465532 | 2.497670 | 10.452699 |
| 512 | 1 | 5 | 104.810185 | 104.400495 到 105.745261 | 2.561158 | 7.801689 |
| 1024 | 1 | 5 | 3773.059512 | 3737.205793 到 3821.183449 | 0.569162 | 35.998978 |
