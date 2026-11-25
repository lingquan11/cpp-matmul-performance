# C0-H02 硬件事实 + 我的纸面预测

- **栏**：动手（Muse 整理读数，预测由我写） | **计划用时**：1.5h | **先读**：C0-R01（CS:APP 第 6 章与 MEM:BLOCKING）
- **一句话目标**：在测量之前，用纸笔预测"分块从多大的 n 开始可能比 ikj 快"，让 H03 成为一次可以被证伪的实验

## 材料（一手来源）
- C0-H01 的 Coreinfo 输出（`results/hw/environment.md`）；Intel ARK 中 Core Ultra 7 255HX 的规格页（只作对照）

## 步骤
1. Muse：新建 `results/experiments/04-scale-sweep/hw-facts.md`，从 Coreinfo 读数填一张表：L1D、L2、L3 的容量与相联度，P 核 / E 核的逻辑处理器编号，L2 由几个核共享，是否支持 AVX2；Coreinfo 截图存入 `04-scale-sweep/hw/`。
2. 【me】在 `04-scale-sweep/prediction.md` 中手算：n = 64 … 2048 时三个矩阵总大小 3·n²·4 B，以及 ikj 中每行 i 都要完整扫一遍的那个矩阵有多大，分别落在哪一级缓存；b = 16、32、64 时 F(b) = 3·b²·4 B 落在哪一级。
3. 【me】写预测：为什么 n ≤ 256 时分块不快？从哪个 n 开始预计分块会比 ikj 快、快多少、哪个 b 最好？什么样的结果会证明预测是错的？
4. 【me】**单独提交 `prediction.md`**，之后不再修改；需要修正时写进 H03 的实验 README。

## 机器验收（开工前写死，AI 不得修改）
- [ ] `hw-facts.md` 每一格都是工具读数，`hw/` 中有截图
- [ ] `prediction.md` 有手算表、一个具体的 n 区间和一个可证伪条件
- [ ] git 中存在一个只包含 `prediction.md` 的 `[me]` 提交，且早于任何测量数据的提交

## 卡住时
- 读不到相联度：写"未读到"，不影响主要预测。
