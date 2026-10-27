# Day 5 学习笔记

今天我学的是矩阵乘法的循环顺序，比较了 `i-j-k` 和 `i-k-j`。

C++ 的二维数组按行存。`i-j-k` 最里面是 k，访问 `B[k][j]` 时每次都换一行，地址跳得比较大。`i-k-j` 最里面是 j，B 的一行和 C 的一行都顺着走，更容易用上缓存行里的空间局部性。

C 的每个元素要把所有 k 的乘积加完，所以要先清零再累加。如果每进入一次 k 就把 C 清零，前面加过的就没了。

今天的双版本测了 64、128、256，每种算法、每种规模 5 次。30 条都是 PASS，逐元素比较都是 YES。按今天这一份 CSV 的中位数，`ijk` 的耗时是 `ikj` 的 6.85 倍、8.45 倍、9.38 倍。数字在 [`experiments/results/2026-10-26-day05-loop-order-local/`](../experiments/results/2026-10-26-day05-loop-order-local/README.md)。

曾有另一份 15 条被写成「昨天」。材料已按职责归档进 src / notes / experiments，验收 ZIP 原件不再入库。用户确认那是 Day 4 的程序导出，在 [`experiments/results/2026-10-26-day04-local/`](../experiments/results/2026-10-26-day04-local/README.md)，不算进今天这 30 条。

学习用时后来补录为 60 分钟。环境和 Day 4 是同一套：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。我提交的双版本源码在 [`src/day05-ijk-ikj-benchmark.cpp`](../src/day05-ijk-ikj-benchmark.cpp)。[`templates/gpt/day05-loop-order-template.cpp`](../templates/gpt/day05-loop-order-template.cpp) 是 GPT 模板，不是我独立写的。
