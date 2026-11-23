# T02 硬件事实 + 纸面预测

- **线**：必做 | **计划用时**：2h | **前置**：T01
- **一句话目标**：先用纸笔预测"分块从多大的 n 开始可能比 ikj 快"，并在测量之前提交，让 T03 成为一次可以被证伪的实验

## 材料（一手来源）
- CS:APP 第 6 章 6.5–6.6（存储器层次、缓存友好代码），以及 Web Aside MEM:BLOCKING：https://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf
- Intel ARK 中 Core Ultra 7 255HX 的规格页（只作对照，以工具读数为准）
- 工具：Sysinternals Coreinfo（`coreinfo64 -c -l`）或 CPU-Z 的 Caches 页

## 步骤
1. 新建 `results/experiments/04-scale-sweep/`。把本目录下的 [t02-hw-facts-draft.md](t02-hw-facts-draft.md) 复制为其中的 `hw-facts.md`，[t02-prediction-draft.md](t02-prediction-draft.md) 复制为 `prediction.md`。
2. 用 Coreinfo 或 CPU-Z 读出：L1D、L2、L3 的容量与相联度；哪些逻辑处理器是 P 核；L2 是每核独享还是几核共享；是否支持 AVX2。截图存到 `04-scale-sweep/hw/`，填进 `hw-facts.md`。
3. 元素类型是 `int`（4 B）。对 n = 64, 128, 256, 512, 1024, 2048 计算：三个矩阵的总大小 3·n²·4 B；在 ikj 中，每处理一行 i 都要完整扫一遍的那个矩阵有多大；两者分别能放进哪一级缓存。
4. 对块大小 b = 16, 32, 64，计算 F(b) = 3·b²·4 B 落在哪一级（`prediction.md` 的 Part 1）。
5. 写预测（`prediction.md` 的 Part 2）：为什么 n ≤ 256 时分块不快？从哪个 n 开始预计分块会比 ikj 快、快多少、哪个 b 最好？什么样的结果会证明预测是错的？
6. **先单独提交这份预测**，之后不再修改；需要修正时，写进 T03 的实验 README。

## 所有权
- `[me]`：全部计算和预测
- `[ai]`：可以用 `results/scripts/block-size-table.py` 核对算术，但必须先手算

## 机器验收（开工前写死，AI 不得修改）
- [ ] `04-scale-sweep/hw/` 中有工具截图；`hw-facts.md` 每一格都是读数，不再有"待填"或〔推断〕
- [ ] `prediction.md` 的 Part 1 和 Part 2 填完，包含一个具体的 n 区间和一个可证伪条件
- [ ] git 中存在一个只包含预测文件的 `[me]` 提交，且早于 T03 的任何测量数据提交

## 闭卷要点
- 缓存行、空间局部性、时间局部性分别对应 ikj 中的哪个访问
- 在 ikj 中，哪个矩阵的复用跨越外层循环？需要多大的缓存才能保住这种复用？
- 分块提高了什么？付出了什么代价（循环开销、内层循环变短）？
- 给出一个新的 n 和 b，现场判断工作集落在哪一级

## 卡住时
- 读不到相联度：写"未读到"，继续往下做，它不影响主要预测。
