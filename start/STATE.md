# STATE（≤ 30 行，只写当前）

- 更新：2026-11-23
- 战役：C0 cpp-matmul 收尾（截止 2026-12-06）→ [campaigns/c0-cpp-wrapup/campaign.md](campaigns/c0-cpp-wrapup/campaign.md)
- 当前任务：T01 三区重构 → [t01-restructure.md](campaigns/c0-cpp-wrapup/t01-restructure.md)
- 任务状态：待我核对（AI 已完成迁移，PR #37）
- 本战役已用：0h / 预算 16–20h

## 上次停在哪
- Day 8 学了 CPU 性能诊断概念，没有实测；之后只有 AI 搭的脚手架，SIMD kernel 没写

## 未解决的问题
- 为什么 n ≤ 256 时分块比 ikj 慢？（假设：工作集太小；T02 预测，T03 验证）
- 本机 L1D/L2/L3 容量还没用工具读过，现有数字都是〔推断〕
- MSVC `/O2` 有没有自动向量化 ikj？（T04）

## 下一步
- 读一遍首页，在 `journey/log.md` 写一行核对结论，以 `[me]` 提交（完成 T01）
- 然后开始 T02：用 Coreinfo 或 CPU-Z 读缓存参数

## 环境
- Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，MSVC 19.51（x64 Release）
- 元素类型 `int`；`blocking.cpp` 的 `MAX_N = 256`，静态二维数组
