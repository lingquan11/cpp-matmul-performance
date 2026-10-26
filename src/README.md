# 我放程序的地方

一个实现一个文件，文件名写出版本，比如朴素三重循环，以后的分块也按策略分开。文件头我会写清思路、矩阵怎么存、怎么编译。能编译过的我才放进来。

`day01-matrix.cpp` 算一个元素，`day02-matrix.cpp` 是固定 2×2 的 i-j-k。

| 文件 | 是什么 |
| --- | --- |
| [day01-matrix.cpp](day01-matrix.cpp) | Day 1：只算 C[0][0] |
| [day02-matrix.cpp](day02-matrix.cpp) | Day 2：固定 2×2 的 i-j-k |
| [day04-matmul.cpp](day04-matmul.cpp) | Day 4：我在对话里粘贴过的单次计时程序。里面有辅导时逐步加上的代码。不是那 15 条正式样本的批量程序 |
| [day05-ijk-ikj-benchmark.cpp](day05-ijk-ikj-benchmark.cpp) | Day 5：我提交的 `ijk` / `ikj` 双版本。按提交内容原样留着，里面不一定每一行都是我自己写的 |

GPT 模板不在这里，在 [`templates/gpt/`](../templates/gpt/README.md)。

本目录是现行成品，文件名用连字符。[`code_history/dayNN/`](../code_history/README.md) 是各日演变快照，快照文件名保持原样。两处并存是正常的，哈希不同不合成一份。

[`code_history/day04/`](../code_history/day04/README.md) 是 Day 4 的版本快照（V01–V07）。V07 是和 Day 4 程序导出一起提交的 `ikj` 批量程序，不放进本目录，也不盖掉 `day04-matmul.cpp`。[`code_history/day05/`](../code_history/day05/README.md) 只留今天双版本的一份复制。现在用的程序仍在本目录。
