# C0-H03 规模扫描测量：n = 256 → 2048

- **栏**：动手（Muse 测量和作图，分析由我写） | **计划用时**：2h | **先读**：C0-R02 | **依赖编程**：C0-P01 通过
- **一句话目标**：在 n = 256 → 2048 上测量 ikj 和分块，用数据回答 H02 的预测是对是错

## 步骤
1. Muse：用 P01 中写好的 `results/harness/scale-sweep.cpp` 和 `results/scripts/run-scale-sweep.bat` 测量 n ∈ {256, 512, 1024, 1536, 2048} × {ikj, blocked_32, blocked_64}，`/O2 /utf-8`，测量条件按战役的统一约定，绑核掩码依据 `hw-facts.md`。
2. Muse：原始 CSV 存入 `04-scale-sweep/data/`；`metadata.md` 记录编译器版本、编译选项、电源模式和绑核掩码；用 P01 中写好的作图脚本作图（横轴 n，纵轴 GOPS）。
3. 【me】按 `start/templates/experiment-readme.md` 写 `04-scale-sweep/README.md`（≤ 30 行）：GOPS 表、图、与 `prediction.md` 逐条对照（命中或落空，以及原因）。

## 机器验收（开工前写死，AI 不得修改）
- [ ] `data/` 中有原始 CSV（5 个 n × 3 个算法 × 5 个样本）；`metadata.md` 字段齐全
- [ ] README 中有 GOPS 表、图，以及"预测 vs 实测"对照，分析部分是 `[me]` 提交

## 卡住时
- n = 2048 单次超过 30 秒：最大规模降到 1536，在 README 中注明。
