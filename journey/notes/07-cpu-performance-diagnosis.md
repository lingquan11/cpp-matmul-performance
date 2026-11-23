# Day 8 学习笔记：CPU 性能诊断与微架构实验

## 我的学习记录
Day 8 我实际学习了 5 小时（300 分钟）。我从缓存、带宽和访问延迟的分析，逐步推进到 MLP、IPC/CPI、Top-down Pipeline Slots、SMT 资源竞争和实验设计。我通过了 Day 8 综合验收。下面记录我需要继续使用的概念、公式和边界。

## 1. 核心模型

\[
T\\approx\\frac{I}{IPC\\times F},\\qquad CPI=\\frac{1}{IPC}
\]

- \(I\)：动态执行架构指令数；
- \(IPC\)：每周期完成的架构指令数；
- \(F\)：有效频率；
- \(T\)：CPU 执行时间近似值。

示例：1,000,000 instructions、IPC=2.0、4 GHz，对应 125 μs；800,000 instructions、IPC=2.1、4 GHz，对应约 95.238 μs。模型预测耗时减少约 23.81%，性能提升倍数约 1.3125（吞吐性能提高约 31.25%）。

模型推算值不能冒充独立墙钟实测值；所有计数必须来自同一目标和同一统计区间。

## 2. LLC 未命中次数与未命中率

- 未命中次数是绝对数量；
- 未命中率是相对比例；
- 未命中率下降不保证未命中总次数下降；
- 必须核对访问总数、事件定义、统计范围和 multiplexing。

## 3. 带宽、延迟与排队

带宽饱和与带宽竞争不等价。即使整体带宽低于可持续上限，也可能存在局部资源竞争、请求排队、突发峰值或访问延迟增加。

带宽是吞吐量维度；Memory Bound 是流水线槽位受内存相关限制的分类。带宽利用率上升与 Memory Bound 下降可以同时出现。

## 4. MLP（Memory-Level Parallelism）

MLP 描述同时处于未完成状态的内存请求的并行程度。硬件最大并行能力不等于程序实际 MLP；数据依赖会限制可并行请求。MLP 增加不意味着整体速度按相同比例增加。

简化排队关系（统计口径一致时）：
\[
N=\\lambda W
\]
N 为平均未完成请求数，λ 为平均完成速率，W 为平均请求延迟。

## 5. Core Bound、Memory Bound 与 SMT

- Memory Bound 增加支持内存相关瓶颈影响增强的解释；
- Core Bound 增加可能与执行端口、计算单元、依赖链、前端或其他核心资源有关；
- LLC 指标不变不能排除 L1/L2、i-cache、uop cache、TLB、请求排队等；
- SMT 兄弟线程可能共享执行端口、ROB、调度器、前端等；
- 不同物理核仍可能共享 LLC、内存控制器和封装功耗预算。

亲和性对照：无后台负载、SMT 兄弟线程、另一个物理核、必要时关闭 SMT。

## 6. Top-down Pipeline Slots

\[
TotalSlots=Cycles\\times SlotsPerCycle
\]

Pipeline Slots 不是 CPU 周期，也不能直接称为 Stall Cycles。一个周期内不同槽位可以有不同归属，槽位数量不能还原成多少周期完全停顿。

IPC 统计架构指令；Retiring Slots 统计退休 uops。近似关系需考虑平均每条指令对应的 uops 数 U：
\[
RetiringRatio\\approx\\frac{IPC\\times U}{SlotsPerCycle}
\]

必须确认百分比以总 slots 还是 Backend Bound 内部为分母，并确认 CPU 微架构事件定义。

## 7. 2×2 因子实验与交互项

A：无压力；B：内存压力；C：计算压力；D：双重压力。

\[
Interaction=T_D-T_B-T_C+T_A
\]

示例 A=100、B=110、C=108、D=130 μs，交互项为 12 μs。乘法独立模型预测 118.8 μs。加法/乘法交互依赖分析尺度，应预先指定。

## 8. perf 计数范围与测量对齐

核对：
- `perf stat -- ./prog`
- `perf stat -p PID`
- `perf stat -t TID`
- `perf stat -a`
- `perf stat -C CPU`

并检查用户态/内核态、context-switches、cpu-migrations、page-faults、multiplexing、SMT 下 Top-down 事件统计范围。

100 μs 单次核心区间可能太短。建议进程/线程内部测量、预热、批量重复、同一事件组同时读 instructions/cycles、避免 multiplexing、随机化顺序、记录频率温度和调度指标。

`task-clock` 对单线程任务可与 wall-clock 对比；多线程时 task-clock 是线程 CPU 时间累计值，可能大于 wall-clock。

## 9. 数据真实性

今天出现的性能表格、带宽、IPC、MLP、延迟、Pipeline Slots 和 A/B/C/D 数字都是教学示例，不是我电脑上的实测数据，不写进正式原始结果 CSV。
