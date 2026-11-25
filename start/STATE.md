# STATE（≤ 30 行；三个小节各自维护）

- 更新：2026-11-25
- 战役：C0 cpp-matmul 收尾（截止 2026-12-06）→ [campaign.md](campaigns/c0-cpp-wrapup/campaign.md)

## 阅读（Muse 记录）
- 当前阅读：C0-R01 CS:APP 第 6 章 + MEM:BLOCKING
- 下一步：读完对 Muse 说"读完 C0-R01"

## 编程（GPT 维护）
- 当前编程任务：C0-P01 大矩阵版 ikj 与分块（等 H02 的预测提交后开始）→ [p01-large-matrix.md](campaigns/c0-cpp-wrapup/p01-large-matrix.md)
- 上次停在哪：C0-T01 三区重构已通过；SIMD kernel 还没写
- 卡点：—

## 动手（Muse 维护）
- 当前动手任务：C0-H02 硬件事实 + 我的纸面预测（读完 R01 后）→ [h02-hw-facts-and-prediction.md](campaigns/c0-cpp-wrapup/h02-hw-facts-and-prediction.md)
- 环境：Windows，Core Ultra 7 255HX；L1D 48/32 KB、L2 3/4 MB、L3 30 MB（Coreinfo 实测）；VS 2026，MSVC 19.51（编译须加 `/utf-8`）；Python 3.13.5 + matplotlib（详见 `results/hw/environment.md`）
- 待解决：n ≤ 256 时分块为什么比 ikj 慢（H02 预测，H03 验证）；MSVC `/O2` 有没有自动向量化 ikj（H04）
