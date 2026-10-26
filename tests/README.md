# 正确性冒烟

现在只有 [`smoke.sh`](smoke.sh)。它用 `g++ -std=c++17 -O2` 编译 `src/` 下现存的 `.cpp`，然后只运行 Day 1 和 Day 2。

- Day 1（`src/day01-matrix.cpp`）输出 `11`
- Day 2（`src/day02-matrix.cpp`）输出 `11 10` 和 `19 30`（每个数后面有一个空格）

[`.github/workflows/compile.yml`](../.github/workflows/compile.yml) 调用这个脚本。没有性能断言，也没有大规模测试矩阵。Day 4、Day 5 只编译，不运行。计时复现：未验证。
