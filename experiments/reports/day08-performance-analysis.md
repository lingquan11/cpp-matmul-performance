# Day 8 性能分析学习报告

> 来源：GPT 归档包整理的学习方法报告，日期未记录。文中数字都是教学示例，不是实测；仓库里没有 Day 8 的结果目录。

## 我的学习记录
我本日学习用时为 5 小时（300 分钟）。我完成了性能诊断方法和微架构分析的综合学习，并通过 Day 8 验收。报告中的数字均为教学示例，不是我电脑的实测结果。

## 摘要
Day 8 从矩阵乘法性能现象推进到 CPU 微架构诊断，覆盖缓存、带宽、延迟、MLP、IPC/CPI、Core Bound/Memory Bound、Top-down Pipeline Slots、SMT 资源竞争、2×2 因子实验和 perf 统计口径。

## 我的表现
我能够提出可检验假设、计算加法和乘法交互模型、识别统计口径问题，并把报告分成事实、支持解释和未知因素。最终综合考核通过。

## 关键公式
\[
T\\approx\\frac{I}{IPC\\times F},\\quad CPI=1/IPC
\]
\[
N=\\lambda W
\]
\[
TotalSlots=Cycles\\times SlotsPerCycle
\]
\[
Interaction=T_D-T_B-T_C+T_A
\]

## 后续真实实验建议
1. 设计无压力、内存压力、计算压力、双重压力的 2×2 实验。
2. 随机化或交替测试顺序，分区组重复。
3. 固定并记录线程亲和性。
4. 对齐目标线程、计数器和计时区间。
5. 记录 instructions、cycles、context-switches、cpu-migrations、page-faults、带宽、频率和温度。
6. 检查事件 multiplexing 和 Top-down 事件分母。

## 数据边界
本日没有可确认的真实硬件性能计数器采集或正式 benchmark CSV。所有数值均为教学示例，不是我电脑上的实测值。本报告是学习方法报告，不是实测性能报告。
