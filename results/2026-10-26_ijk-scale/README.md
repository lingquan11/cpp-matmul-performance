# i-j-k 三重循环规模扫描

测了什么：i-j-k 三重循环。实现与 `src/day02_matrix.cpp` 相同，上界为参数 n。Day 1 的程序计算单个元素 C[0][0]。

测量时源文件记为 `benchmarks/ijk_kernel.cpp`，现位于 `benchmarks/kernels/ijk.cpp`。本目录的 CSV 和图对应该次编译。解读见 [实验记录](../../docs/report.md)。

条件摘要：

- 时间：2026-10-26T04:05:29Z
- 系统：Ubuntu 24.04.5 LTS，Linux 6.12.94+，KVM 云端 VM
- CPU：`lscpu` 型号 Intel(R) Xeon(R) Processor（family 6，model 207，stepping 2），4 vCPU；L1d 192 KiB，L1i 128 KiB，L2 8 MiB，L3 320 MiB
- 内存：MemTotal 16387280 kB
- 编译器：g++ (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0
- 编译命令：`g++ -O2 -std=c++17 -Wall -Wextra -Werror -o build/matmul_bench benchmarks/matmul_bench.cpp benchmarks/ijk_kernel.cpp`
- 图中字体：WenQuanYi Micro Hei (/usr/share/fonts/truetype/wqy/wqy-microhei.ttc)

计时对象是结果元素清零和 i-j-k 乘加。输入构造、校准和正确性检查不在正式样本里。单次调用短于设定的最短样本时长时，同一样本内连续重复，单次耗时 = 样本总耗时 / 重复次数。GFLOPS = 2·n³ / 单次耗时秒 / 10⁹，把每次乘加计为 1 次乘法和 1 次加法。内核是 32 位整数，与现有实现一致；数字只适合相对比较，不是浮点峰值。

原始数据：`raw.csv`。汇总：`summary.csv`。图：`perf.png`。完整环境：`environment.txt`。

| 规模 n | 样本内重复 | 正式样本 | 单次耗时中位数 (ms) | 单次耗时最小–最大 (ms) | GFLOPS（中位数耗时） | 相对上一档耗时倍数 |
| --- | ---: | ---: | ---: | --- | ---: | ---: |
| 64 | 1322 | 5 | 0.075661 | 0.075447–0.077330 | 6.929414 | — |
| 128 | 79 | 5 | 1.285246 | 1.283210–1.333168 | 3.263424 | 16.986855 |
| 256 | 8 | 5 | 13.434294 | 13.413632–13.465532 | 2.497670 | 10.452699 |
| 512 | 1 | 5 | 104.810185 | 104.400495–105.745261 | 2.561158 | 7.801689 |
| 1024 | 1 | 5 | 3773.059512 | 3737.205793–3821.183449 | 0.569162 | 35.998978 |
