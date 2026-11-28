# 学习笔记：循环顺序与缓存

对应 [第 2 章](../02-loop-order.md)；按日期排列，内容保持原样。

## Day 5 学习笔记

今天我学的是矩阵乘法的循环顺序，比较了 `i-j-k` 和 `i-k-j`。

C++ 的二维数组按行存。`i-j-k` 最里面是 k，访问 `B[k][j]` 时每次都换一行，地址跳得比较大。`i-k-j` 最里面是 j，B 的一行和 C 的一行都顺着走，更容易用上缓存行里的空间局部性。

C 的每个元素要把所有 k 的乘积加完，所以要先清零再累加。如果每进入一次 k 就把 C 清零，前面加过的就没了。

今天的双版本测了 64、128、256，每种算法、每种规模 5 次。30 条都是 PASS，逐元素比较都是 YES。按今天这一份 CSV 的中位数，`ijk` 的耗时是 `ikj` 的 6.85 倍、8.45 倍、9.38 倍。数字在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../../results/experiments/01-loop-order/)。

曾有另一份 15 条被写成「昨天」。材料已按职责归档进 src / notes / experiments，验收 ZIP 原件不再入库。用户确认那是 Day 4 的程序导出，在 [`experiments/results/2026-10-26-day04-local/`](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/experiments/results/2026-10-26-day04-local/README.md)，不算进今天这 30 条。

学习用时后来补录为 60 分钟。环境和 Day 4 是同一套：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。我提交的双版本源码在 [`src/day05-ijk-ikj-benchmark.cpp`](../../results/src/loop-order.cpp)。[`templates/gpt/day05-loop-order-template.cpp`](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是我独立写的。

## Day 6 学习笔记

今天我学习了二维数组在内存中的排列方式，以及连续访问和跨行访问对缓存局部性的影响。

对于 `int B[256][256]`，假设每个 `int` 占 4 字节，`B[0][0]` 的地址是 p，那么 `B[i][j]` 的地址是：

`p + (i * 256 + j) * 4`

例如 `B[3][5]` 的地址是 `p + 3092`。每行有 256 个整数，所以每行跨度是 1024 字节。

在假设缓存行大小为 64 字节的情况下，一条缓存行可以容纳 16 个 `int`。`ikj` 在内层循环中连续访问 `B[k][j]`，相邻地址只差 4 字节；`ijk` 在内层循环中访问 `B[k][j]` 时，k 每次增加 1，地址会跨越 1024 字节。连续访问更容易利用空间局部性，但不能保证每次访问都命中缓存。

我还理解了，实测发现 `ikj` 更快，并不能单独证明它的缓存未命中次数更少。要进一步验证原因，需要结合硬件、编译器、编译参数和性能计数器等证据。

循环重排改变的是数据访问顺序；分块计算则把大矩阵拆成小块，让当前正在处理的数据有更多机会留在缓存中并被重复使用。分块大小仍然需要通过实验验证。

今天没有编写新的代码。下一步我会学习分块矩阵乘法的索引结构，先从小规模例子理解每个子块如何参与计算，再考虑实现和性能测量。
