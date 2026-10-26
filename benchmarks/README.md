# benchmarks/

计时驱动和脚本。各内核用同一套测量画到一张图上。

## 一条命令

```bash
python3 benchmarks/run_bench.py
```

脚本会：

1. 扫描 `benchmarks/kernels/*.cpp`
2. 生成 `build/kernel_registry.cpp`（构建产物，不进入仓库）
3. 编译并先做正确性检查
4. 对每个实现、每个规模做正式测量
5. 把所有实现画进同一张 `perf.png`

查看当前内核：

```bash
python3 benchmarks/run_bench.py --list
```

## 当前内核

`kernels/ijk.cpp`：与 `src/day02_matrix.cpp` 相同的 i-j-k 循环，上界为参数 `n`。

## 加入新版本

在 `benchmarks/kernels/` 新增一个文件，文件名与 id 一致，例如 `kernels/ikj.cpp`：

```cpp
// BENCH_ID: ikj
// BENCH_LABEL: i-k-j 循环重排
// BENCH_LABEL_EN: i-k-j loop order

void matmul_ikj(const int* __restrict__ A, const int* __restrict__ B,
                int* __restrict__ C, int n) {
    // 循环写在这里。
}
```

约定：

- `BENCH_ID` 由字母、数字、下划线组成，并与函数名 `matmul_<id>` 一致
- `BENCH_LABEL` 是图例中的中文名；`BENCH_LABEL_EN` 可选，系统没有中文字体时使用
- 目录中的每个 `.cpp` 都会参与编译，放进来的文件需要能通过下面的检查
- A、B、C 是行主序的 n×n `int` 矩阵，三者地址不重叠
- 函数先把 C 的每个元素写成 0，再累加。同一样本会连续调用多次，调用之间不再另做清零
- 按文件名排序。一个实现的全部规模测完，再测下一个
- 计时编译选项为 `-O2 -std=c++17 -Wall -Wextra -Werror -falign-loops=32`，内核与计时循环分文件编译，不使用 LTO，这样多次调用会逐次执行。`-falign-loops=32` 使各内核的循环使用相同的对齐。在本仓库的云端 VM 上，两份相同循环因链接位置不同，n=64 大约测得 3.87 与 6.94 GFLOPS；加上该选项后大约为 6.84 与 6.85（各 2 个样本）。2026-10-26 主表的编译命令见 `results/2026-10-26_ijk-scale/environment.txt`

加入之后仍用同一条命令。每个实现都会做这些对照：

- n=2，Day 2 的 A、B，结果为 `11 10 / 19 30`
- n=2，B 为单位矩阵，结果为 A
- n=2，B 为零矩阵，结果为 0
- n=4，A 和 B 全是 1，结果全是 4
- n=3，固定小整数输入，校验和与逐项乘加一致

检查通过后才写入性能数据。带 `main` 的学习程序放在 `src/`，基准扫描 `benchmarks/kernels/`。

## 计时口径

- 时钟：`std::chrono::steady_clock`
- 正式样本计实现函数的连续调用
- 输入构造、校准、正确性检查和打印在样本之外
- 单次调用短于 `--min-ms`（默认 100）时，在同一样本内重复，单次耗时 = 样本总耗时 / 重复次数
- 每个规模默认 5 个正式样本。图上的点是中位数，误差线是最小和最大
- GFLOPS = `2 * n^3 / 单次耗时秒 / 1e9`，一次乘法加一次加法，按 32 位整数核折算，用于同一次实验内的比较

比较循环顺序或分块时，保持编译选项、规模、输入和上述口径不变，每次增加一个实现文件。
