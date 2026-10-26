# Day 5 学习笔记

今天我学的是矩阵乘法的循环顺序，比较了 `i-j-k` 和 `i-k-j`。

C++ 的二维数组按行存。`i-j-k` 最里面是 k，访问 `B[k][j]` 时每次都换一行，地址跳得比较大。`i-k-j` 最里面是 j，B 的一行和 C 的一行都顺着走，更容易用上缓存行里的空间局部性。

C 的每个元素要把所有 k 的乘积加完，所以要先清零再累加。如果每进入一次 k 就把 C 清零，前面加过的就没了。

今天的双版本测了 64、128、256，每种算法、每种规模 5 次。30 条都是 PASS，逐元素比较都是 YES。按今天这一份 CSV 的中位数，`ijk` 的耗时是 `ikj` 的 6.85 倍、8.45 倍、9.38 倍。数字在 [`experiments/results/2026-10-26_day05-loop-order/`](../experiments/results/2026-10-26_day05-loop-order/README.md)。

昨天的 15 条我也补进来了，全部 PASS。那是另一天、只测 `ikj` 的程序，和今天分开放，不混成同一轮。

学习用时未记录。我提交的双版本源码在 [`src/day05_ijk_ikj_benchmark.cpp`](../src/day05_ijk_ikj_benchmark.cpp)。[`src/gpt-templates/day05_loop_order_comparison.cpp`](../src/gpt-templates/day05_loop_order_comparison.cpp) 是 GPT 模板，不是我独立写的。
