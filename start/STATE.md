# STATE（≤ 30 行，只写当前）

- 更新：2026-11-25
- 战役：C0 cpp-matmul 收尾（截止 2026-12-06）→ [campaigns/c0-cpp-wrapup/campaign.md](campaigns/c0-cpp-wrapup/campaign.md)
- 当前任务：C0-T02 硬件事实 + 纸面预测 → [t02-hw-facts-and-prediction.md](campaigns/c0-cpp-wrapup/t02-hw-facts-and-prediction.md)
- 本战役已用：5 分钟 / 预算 16–20h

## 上次停在哪
- C0-T01 通过；首页已补上当前位置与下一步的入口。
- Day 8 学了 CPU 性能诊断概念，没有实测；之后只有 AI 搭的脚手架，SIMD kernel 没写

## 未解决的问题
- 为什么 n ≤ 256 时分块比 ikj 慢？（假设：工作集太小；T02 预测，T03 验证）
- 本机 L1D/L2/L3 容量还没用工具读过，现有数字都是〔推断〕
- MSVC `/O2` 有没有自动向量化 ikj？（T04）

## 下一步
- C0-S01 尚未就绪，先对 Muse 说“做 C0-S01”，准备本机工具。
- 环境就绪后开始 C0-T02：读取本机缓存参数，手算后提交分块性能预测。

## 环境
- Windows（10.0.26200.9974），Intel Core Ultra 7 255HX；缓存（Coreinfo 实测）：L1D 48/32 KB、L1I 64 KB、L2 3/4 MB、L3 30 MB，行 64 B
- Visual Studio 2026 Community 18.11.1，MSVC 19.51.36260（x64）；Python 3.13.5，matplotlib 3.10.0；Coreinfo64 已备（全路径运行）
- C0-S01 已就绪（2026-11-25，验收记录见 `results/hw/environment.md`）；本机编译 `blocking.cpp` 须加 `/utf-8`
- 元素类型 `int`；`blocking.cpp` 的 `MAX_N = 256`，静态二维数组
