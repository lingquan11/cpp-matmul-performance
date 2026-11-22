# STATE（≤30 行，只写当前）

- 更新：2026-11-22
- 战役：cpp-matmul 收尾 v1.0（截止 2026-12-06），见 CAMPAIGN.md
- 当前任务：T01 盘点与冻结 → tasks/T01.md
- 任务状态：待我核对（AI 已按默认表格执行归档，分支 `workflow/t01-freeze`）
- 本战役已用：0h / 预算 16–20h

## 上次停在哪
- Day 8 学了 CPU 性能诊断概念（缓存、IPC、Memory Bound），没有实测
- 11-02 之后只增加了 AI 脚手架；SIMD、LDA 和 6 种循环顺序的 kernel 都没写

## 未解决的问题
- 为什么 n = 64/128/256 时分块比 ikj 慢？（假设：工作集太小，T02 预测、T03 验证）
- 本机 L1D/L2/L3 容量还没用工具读过，现有数字都是〔推断〕
- MSVC `/O2` 有没有自动向量化 ikj？（T04）

## 下一步
- 打开仓库首页读一遍，确认归档结果；在 log 里写一句归档了什么、为什么不做
- 然后开始 T02：用 Coreinfo 或 CPU-Z 读出缓存参数

## 环境
- Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，MSVC 19.51（x64 Release）
- 矩阵元素类型：`int`（int32），当前 `MAX_N = 256`，用静态二维数组
