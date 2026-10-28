# 我放程序的地方

一个实现一个文件，文件名写出版本，比如朴素三重循环，以后的分块也按策略分开。文件头我会写清思路、矩阵怎么存、怎么编译。能编译过的我才放进来。

`day01-matrix.cpp` 算一个元素，`day02-matrix.cpp` 是固定 2×2 的 i-j-k。

| 文件 | 是什么 |
| --- | --- |
| [day01-matrix.cpp](day01-matrix.cpp) | Day 1：只算 C[0][0] |
| [day02-matrix.cpp](day02-matrix.cpp) | Day 2：固定 2×2 的 i-j-k |
| [day04-matmul.cpp](day04-matmul.cpp) | Day 4：我在对话里粘贴过的单次计时程序。里面有辅导时逐步加上的代码。不是那 15 条正式样本的批量程序 |
| [day05-ijk-ikj-benchmark.cpp](day05-ijk-ikj-benchmark.cpp) | Day 5：我提交的 `ijk` / `ikj` 双版本。按提交内容原样留着，里面不一定每一行都是我自己写的 |
| [day07-matrix-blocking.cpp](day07-matrix-blocking.cpp) | Day 7：根据学习对话整理的 GPT 整合版。不是用户本机逐字源码，也不是 [`code_history/day07/V01_user_submitted_intermediate.cpp`](../code_history/day07/V01_user_submitted_intermediate.cpp)。归档生成环境曾用 `g++ -std=c++17 -O2 -Wall -Wextra -pedantic` 验证边界与 75 条 PASS；未在用户本机 Visual Studio 重新构建。不要用新跑结果覆盖正式 CSV |

GPT 模板不在这里，在 [`templates/gpt/`](../templates/gpt/README.md)。Day 7 的可编译整合版放在本目录，是因为补缴时需要一份带完整 `main` 的文件；文件头写明它是 GPT 整合版，不能把它读成用户原始提交。

本目录放这一阶段接着用的程序，文件名用连字符。Day 7 的 `day07-matrix-blocking.cpp` 是标明来源的 GPT 整合版，不把它算作用户逐字提交的成品。[`code_history/dayNN/`](../code_history/README.md) 是各日演变快照，快照文件名保持原样。两处并存是正常的，哈希不同不合成一份。

[`code_history/day04/`](../code_history/day04/README.md) 是 Day 4 的版本快照（V01–V07）。V07 是和 Day 4 程序导出一起提交的 `ikj` 批量程序，不放进本目录，也不盖掉 `day04-matmul.cpp`。[`code_history/day05/`](../code_history/day05/README.md) 只留今天双版本的一份复制。现在用的程序仍在本目录。

## 2026-10-29 验证补充

上述来源与历史验证描述保留。现行文件已在本机 MSVC 构建；Day 7 的独立正确性测试覆盖 392 个组合，不执行 benchmark main，不复现历史 CSV。[测试入口](../tests/README.md) · [环境与维护记录](../notes/maintenance-2026-10-29.md)。
