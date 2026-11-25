# C0-P01 大矩阵版 ikj 与分块

- **栏**：编程 | **计划用时**：2.5h | **先读**：C0-R01（CS:APP 第 6 章） | **依赖动手**：C0-H01（已完成）
- **一句话目标**：把 ikj 和分块搬到能跑 n = 2048 的版本上，为 H03 的规模扫描准备好经过验证的 kernel
- **前提**：H02 的预测已单独提交（预测要早于任何测量）

## 材料（一手来源）
- `results/src/blocking.cpp`（旧版，`MAX_N = 256` 的静态数组，**不要修改**：实验 03 记录了它的哈希）

## 步骤
1. 【me】新建 `results/src/large-matrix.hpp`：`std::vector<int>` 行主序（下标 `i*n + j`），n 运行时传入（最大 2048）；写 `fill_inputs`、`matmul_ikj` 和 `matmul_blocked(n, b)`。
2. 【me】推导：按你的初始化方式，n = 2048 时 C 的最大元素会不会超出 `int`？写进 `results/experiments/04-scale-sweep/README.md` 的开头；会溢出就换更小的输入值。
3. 【me】正确性测试 `results/tests/large-matrix-correctness.cpp`：n ≤ 512 时与朴素 ijk 逐元素比较；n > 512 时分块与 ikj 比较；包含非整除情况（n = 1000、b = 64）。
4. 【me】错误注入：把分块的边界条件改错，确认测试报 FAIL，再改回。
5. 【ai】把测试接入 `results/tests/smoke.sh`；写计时框架 `results/harness/scale-sweep.cpp`：include 我的头文件，参数 `--n --algo {ikj,blocked,ikj_avx2} --block --runs`，预热 1 次、正式 5 次，CSV 字段 n、algorithm、block、sample、ms、gops、check；只校验一次结果（n ≤ 512 对照朴素 ijk，更大时对照 ikj），不在每个样本里重算；再写 `results/scripts/run-scale-sweep.bat`（绑核掩码作参数）和作图脚本。

## 机器验收（开工前写死，AI 不得修改）
- [ ] `large-matrix-correctness` 全部 PASS（含 n = 1000、b = 64），错误注入那次 FAIL
- [ ] 已接入 smoke.sh，CI 为绿色
- [ ] 溢出推导已写入 `04-scale-sweep/README.md`

## 卡住时
- 超过 4h：先只做 ikj 和 b = 32，n = 1000 的非整除测试保留。
