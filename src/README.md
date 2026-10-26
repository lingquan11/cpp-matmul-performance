# 我放程序的地方

一个实现一个文件，文件名写出版本，比如朴素三重循环，以后的分块也按策略分开。文件头我会写清思路、矩阵怎么存、怎么编译。能编译过的我才放进来。

`day01_matrix.cpp` 算一个元素，`day02_matrix.cpp` 是固定 2×2 的 i-j-k。要进性能图的实现我放到 `benchmarks/kernels/`，约定在 `benchmarks/README.md`。

| 文件 | 是什么 |
| --- | --- |
| [day01_matrix.cpp](day01_matrix.cpp) | Day 1：只算 C[0][0] |
| [day02_matrix.cpp](day02_matrix.cpp) | Day 2：固定 2×2 的 i-j-k |
| [day04_matmul.cpp](day04_matmul.cpp) | Day 4：我在对话里粘贴过的单次计时程序。里面有辅导时逐步加上的代码 |
| [gpt-templates/day04_benchmark.cpp](gpt-templates/day04_benchmark.cpp) | GPT 模板，不是我独立写的。说明在 [gpt-templates/README.md](gpt-templates/README.md) |

`gpt-templates/` 只放辅导时的模板，不算我独立写完的实现。

[`code_history/day04/`](../code_history/day04/README.md) 是 Day 4 的版本快照（V01–V06）。现在用的程序仍在本目录。
