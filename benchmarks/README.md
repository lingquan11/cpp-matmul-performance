# benchmarks/

性能测量的驱动和脚本在这里。算法本身仍然由人写；这个目录负责用同一套计时把它们测完、画到一张图上。

## 一条命令

```bash
python3 benchmarks/run_bench.py
```

脚本会：

1. 扫描 `benchmarks/kernels/*.cpp`
2. 生成 `build/kernel_registry.cpp`（构建产物，不进仓库）
3. 编译并先跑正确性检查
4. 对每个实现、每个规模做正式测量
5. 把所有实现画进同一张 `perf.png`

只查看发现了哪些实现：

```bash
python3 benchmarks/run_bench.py --list
```

## 现在有什么

只有 `kernels/ijk.cpp`。它是 `src/day02_matrix.cpp` 的 i-j-k 循环，上界从写死的 2 改成参数 `n`。循环重排、分块、脉动阵列都还没有文件。

## 以后加一个新版本

新写一个文件，放进 `benchmarks/kernels/`，文件名建议和 id 一致，例如 `kernels/ikj.cpp`：

```cpp
// BENCH_ID: ikj
// BENCH_LABEL: i-k-j 循环重排
// BENCH_LABEL_EN: i-k-j loop order

void matmul_ikj(const int* __restrict__ A, const int* __restrict__ B,
                int* __restrict__ C, int n) {
    // 在这里写你自己的循环。
}
```

约定：

- `BENCH_ID` 只能含字母、数字、下划线，并且和函数名 `matmul_<id>` 一致
- `BENCH_LABEL` 是图例里的中文名；`BENCH_LABEL_EN` 可选，没有中文字体时用它
- 这个目录里的每一个 `.cpp` 都会被编译。没写完的文件不要放进来
- A、B、C 是行主序的 n×n `int` 矩阵，三者不要重叠
- 函数要自己把 C 的每个元素先写成 0 再累加。同一样本里会连续调用多次，两次调用之间不会另外清零
- 按文件名排序。测量顺序是一个实现的全部规模跑完，再跑下一个
- 计时编译使用 `-O2 -std=c++17 -Wall -Wextra -Werror -falign-loops=32`，不要加 `-flto`。内核和计时循环分开放，是为了避免多次调用被编译器折成一次。`-falign-loops=32` 让不同实现的循环按同样的边界对齐；在这台云端 VM 上，两份相同循环仅仅因为链接位置不同，n=64 会测出大约 3.87 和 6.94 GFLOPS，加上该选项后都在 6.84 附近（各 2 个样本的另一次检查，不是首页那张表）

加完之后还是原来那条命令。正确性检查会对每个实现做这些对照：

- n=2，Day 2 的 A、B，结果应为 `11 10 / 19 30`
- n=2，B 为单位矩阵，结果应为 A
- n=2，B 为零矩阵，结果应为 0
- n=4，A 和 B 全是 1，结果应全是 4
- n=3，固定的小整数输入，校验和与逐项乘加一致

检查失败就不会写性能结论。带 `main` 的学习程序可以继续放在 `src/`，基准不会扫描 `src/`。

## 计时口径

- 时钟：`std::chrono::steady_clock`
- 计入正式样本的只有实现函数本身的连续调用
- 输入构造、校准、正确性检查和打印不在正式样本里
- 单次调用短于 `--min-ms`（默认 100）时，同一样本内重复，单次耗时 = 样本总耗时 / 重复次数
- 每个规模默认 5 个正式样本。图上的点是中位数，误差线是最小和最大
- GFLOPS = `2 * n^3 / 单次耗时秒 / 1e9`。一次乘法加一次加法。这是 32 位整数核，不是浮点峰值

控制变量：比较循环顺序或分块时，保持编译选项、规模、输入和上面的口径不变，一次只增加一个实现文件。
