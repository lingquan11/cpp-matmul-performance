# T03 扩大规模实测：n = 256 → 2048

- **线**：必做 | **计划用时**：4h | **前置**：T02 的预测已提交
- **一句话目标**：在 n = 256 → 2048 上测量 ikj 和分块，用数据回答 T02 的预测是对是错

## 步骤
1. 【me】新建 `results/src/large-matrix.cpp`：用 `std::vector<int>` 按行主序存储（下标 `i*n + j`），运行时传入 n（最大 2048）。重写 `matmul_ikj` 和 `matmul_blocked(n, b)`。**不要修改 `blocking.cpp`**（实验 03 记录了它的哈希）。
2. 【me】推导：按你的初始化方式，n = 2048 时 C 的最大元素会不会超出 `int` 范围？写进实验 README。会溢出就换更小的输入值。
3. 【ai】计时框架：可以复用 `results/harness/scale-sweep.cpp`。每个配置预热 1 次、正式 5 次、取中位数；输出 CSV（n、算法、b、每次耗时、GOPS、校验结果）。GOPS = 2·n³ / 耗时（int 运算，不叫 GFLOPS）。
4. 【me】正确性测试 `results/tests/large-matrix-correctness.cpp`：n ≤ 512 时与朴素 ijk 逐元素比较；n > 512 时分块与 ikj 比较；包含非整除情况（n = 1000、b = 64）。接入 `smoke.sh`。
5. 【me】测量条件：接通电源，电源模式选"最佳性能"，关闭其他程序，用 `start /high /affinity <掩码>` 绑定到一个 P 核（掩码依据 T02 的 Coreinfo 结果）。写进 `metadata.md`。
6. 【me】配置：n ∈ {256, 512, 1024, 1536, 2048} × {ikj, blocked_32, blocked_64}，`/O2`。
7. 【ai】作图：横轴 n，纵轴 GOPS，每个算法一条线。
8. 【me】按 `start/templates/experiment-readme.md` 写 `04-scale-sweep/README.md`（≤ 30 行）：结果表、图、与 T02 预测逐条对照（命中或落空，以及原因）。

## 所有权
- `[me]`：两个 kernel、溢出推导、正确性测试、测量、README 中的分析
- `[ai]`：计时框架、CSV 输出、作图

## 机器验收（开工前写死，AI 不得修改）
- [ ] 正确性测试全部 PASS（含 n = 1000、b = 64），已接入 smoke.sh，CI 为绿色
- [ ] `04-scale-sweep/data/` 中有原始 CSV（5 个 n × 至少 3 个算法 × 5 个样本）；`metadata.md` 记录编译器版本、编译选项、电源模式和绑核掩码
- [ ] README 中有 GOPS 表、图，以及"预测 vs 实测"对照

## 闭卷要点
- 在你的数据中，哪个 n 是转折点？它与 L2、L3 的容量是什么关系？
- 为什么小 n 时分块反而更慢？至少说出 2 个原因
- 为什么要绑定到 P 核？不绑定会怎样？
- 只看耗时，能断定"缓存未命中减少了"吗？还缺什么证据（→ B01）
- 把元素换成 `double`，转折点往哪个方向移动？大约移动多少？

## 卡住时
- n = 2048 单次超过 30 秒：最大规模降到 1536，在 README 中注明。
- 超过 6h：只保留 ikj 和 blocked_32。
