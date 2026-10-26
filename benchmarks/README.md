# 我怎么跑基准

各内核用同一套测量，画到一张图上。

```bash
python3 benchmarks/run_bench.py
```

这条命令会去看 `benchmarks/kernels/*.cpp`，写出 `build/kernel_registry.cpp`（这是构建时生成的，我不放进仓库），编译，先做正确性检查，再对每个实现、每个规模做正式测量，最后画进同一张 `perf.png`。

只看现在有哪些内核：

```bash
python3 benchmarks/run_bench.py --list
```

现在的内核是 `kernels/ijk.cpp`，和 `src/day02_matrix.cpp` 一样的 i-j-k，上界是参数 `n`。

我之后要加版本，就在 `benchmarks/kernels/` 里新放一个文件，文件名和 id 一致。比如 `kernels/ikj.cpp`：

```cpp
// BENCH_ID: ikj
// BENCH_LABEL: i-k-j 循环重排
// BENCH_LABEL_EN: i-k-j loop order

void matmul_ikj(const int* __restrict__ A, const int* __restrict__ B,
                int* __restrict__ C, int n) {
    // 我把循环写在这里。
}
```

`BENCH_ID` 用字母、数字和下划线，并且跟函数名 `matmul_<id>` 一样。`BENCH_LABEL` 是图例里的中文名。`BENCH_LABEL_EN` 可以不写，机器上没有中文字体时我用它。这个目录里每个 `.cpp` 都会参加编译，所以放进来的文件得能通过下面的检查。A、B、C 是行主序的 n×n `int` 矩阵，三块地址不重叠。函数先把 C 的每个元素写成 0 再累加。同一样本会连着调用很多次，调用之间我不再另外清零。实现按文件名排序，一个实现的全部规模测完再测下一个。

计时编译是 `-O2 -std=c++17 -Wall -Wextra -Werror -falign-loops=32`。内核和计时循环分文件编译，我不用 LTO，这样多次调用会一次次执行。`-falign-loops=32` 是为了让各内核的循环用同样的对齐。我在这台云端 VM 上试过，两份相同循环因为链接位置不同，n=64 大约测到 3.87 和 6.94 GFLOPS；加上这个选项后大约是 6.84 和 6.85，各 2 个样本。2026-10-26 主表的编译命令在 `results/2026-10-26_ijk-scale/environment.txt`。

加进去之后还是同一条命令。每个实现我都会对一下：n=2 用 Day 2 的 A、B，结果应是 `11 10 / 19 30`；n=2 且 B 是单位矩阵，结果应是 A；n=2 且 B 是零矩阵，结果应是 0；n=4 且 A 和 B 全是 1，结果应全是 4；n=3 用固定的小整数，校验和要和逐项乘加一致。检查过了才写性能数据。带 `main` 的学习程序我放在 `src/`，基准只扫 `benchmarks/kernels/`。

时钟用 `std::chrono::steady_clock`。正式样本计的是实现函数被连续调用的时间。构造输入、校准、正确性检查和打印都在样本外面。一次调用短于 `--min-ms`（默认 100）时，我在同一样本里重复，单次耗时 = 样本总耗时 / 重复次数。每个规模默认 5 个正式样本。图上的点是中位数，误差线是最小和最大。GFLOPS = `2 * n^3 / 单次耗时秒 / 1e9`，一次乘法加一次加法，按 32 位整数核折算，用来在同一次实验里比较。

我比较循环顺序或分块的时候，编译选项、规模、输入和这个口径保持不变，每次只多加一个实现文件。
