# 战役 C0：cpp-matmul 收尾 v1.0

- **周期**：2026-11-23 → 2026-12-06（截止日不延期）
- **预算**：每周 8–10h，合计约 18.5h（阅读 5h + 编程 5h + 动手 8.5h）；三区重构 C0-T01 已于 2026-11-25 通过，不再列入
- **唯一交付物**：`v1.0` Release。`results/README.md` 用一张表回答"循环顺序、分块、SIMD 在不同规模下各带来什么"，并附 `results/summary.md`
- **主线**：读 Algorithmica → 编程 P01–P02 → 动手 H01–H05（三张表，计入进度）
- **支线**（选做，不计入进度）：[Performance Ninja](https://github.com/dendibakh/perf-ninja)，Denis Bakhvalov 的免费性能调优实验课，每个实验配讲解视频，和主线读到的章节一一对应
- **主教材**：Sergey Slotin, [*Algorithms for Modern Hardware*](https://en.algorithmica.org/hpc/)（Algorithmica，免费在线书）；阅读表全部出自它
- **统一约定**：元素类型 `int`；计时只在我说"开始测量"时运行；每个配置预热 1 次、正式 5 次取中位数；接通电源、"最佳性能"模式、绑定到一个 P 核；吞吐量用 GOPS = 2·n³ / 耗时（int 运算，不叫 GFLOPS）
- **推荐顺序**：R01 → R02 → H02（预测先提交）→ P01 → H03 → R03 → R04 → P02 → H04 → H05
- **之后**：[ai-infra-plan](https://github.com/lingquan11/ai-infra-plan)（36 周 AI Infra 计划，代码放在 [AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning)）

## 阅读（我自己读，不和 AI 对话；读完对 Muse 说"读完 <ID>"；状态：未读 / 在读 / 读完）
| ID | 长文 | 读哪些部分 | 预计用时 | 读完应该能回答（自测，不考核） | 状态 |
|---|---|---|---:|---|---|
| C0-R01 | Algorithmica 第 8 章 [External Memory](https://en.algorithmica.org/hpc/external-memory/) | [Memory Hierarchy](https://en.algorithmica.org/hpc/external-memory/hierarchy/)、[External Memory Model](https://en.algorithmica.org/hpc/external-memory/model/)、[Cache-Oblivious Algorithms](https://en.algorithmica.org/hpc/external-memory/oblivious/)、[Spatial and Temporal Locality](https://en.algorithmica.org/hpc/external-memory/locality/) | 1.5h | 为什么用"读写了多少个缓存块"而不是运算次数来衡量？矩阵乘法的 cache-oblivious 递归和手写分块是什么关系？空间局部性、时间局部性分别对应 ikj 中的哪个访问？ | 未读 |
| C0-R02 | Algorithmica 第 9 章 [RAM & CPU Caches](https://en.algorithmica.org/hpc/cpu-cache/) | [Memory Bandwidth](https://en.algorithmica.org/hpc/cpu-cache/bandwidth/)、[Memory Latency](https://en.algorithmica.org/hpc/cpu-cache/latency/)、[Cache Lines](https://en.algorithmica.org/hpc/cpu-cache/cache-lines/)、[Prefetching](https://en.algorithmica.org/hpc/cpu-cache/prefetching/)、[Cache Associativity](https://en.algorithmica.org/hpc/cpu-cache/associativity/) | 1.5h | 带宽受限和延迟受限怎样区分？为什么步长是 2 的幂时可能突然变慢？硬件预取对 ikj 的内层循环意味着什么？ | 未读 |
| C0-R03 | Algorithmica 第 10 章 [SIMD Parallelism](https://en.algorithmica.org/hpc/simd/) | 章首、[Intrinsics and Vector Types](https://en.algorithmica.org/hpc/simd/intrinsics/)、[Moving Data](https://en.algorithmica.org/hpc/simd/moving/)、[Auto-Vectorization and SPMD](https://en.algorithmica.org/hpc/simd/auto-vectorization/) | 1h | 一条 AVX2 指令一次处理几个 int32？未对齐加载有什么代价？编译器在什么条件下才会自动向量化？ | 未读 |
| C0-R04 | Algorithmica 第 11 章 [Matrix Multiplication](https://en.algorithmica.org/hpc/algorithms/matmul/) | 全文 | 1h | 转置、向量化、寄存器复用的微内核、分块各解决了什么瓶颈？这能不能解释我的分块没有变快？ | 未读 |

## 编程（GPT 带写，只看机器验收；状态：未开始 / 进行中 / 通过）
| ID | 任务 | 计划用时 | 验收要点（展开任务卡时不得弱于此） | 先读 | 依赖动手 | 状态 |
|---|---|---:|---|---|---|---|
| C0-P01 | 大矩阵版 ikj 与分块 | 2.5h | 见 [p01-large-matrix.md](p01-large-matrix.md) | C0-R01 | C0-H01 | 未开始 |
| C0-P02 | 手写 AVX2 版 ikj | 2.5h | 见 [p02-avx2.md](p02-avx2.md) | C0-R03、R04 | C0-H01 | 未开始 |

## 动手（Muse 负责，只看机器验收；状态：未开始 / 进行中 / 完成）
| ID | 任务 | 计划用时 | 验收要点 | 先读 | 状态 |
|---|---|---:|---|---|---|
| C0-H01 | 环境：Coreinfo、MSVC 命令行、Python + matplotlib | 0.5h | `coreinfo64 -c -l` 有输出；`cl` 能编译 `results/src/blocking.cpp`（须加 `/utf-8`）；作图脚本能运行；记录见 `results/hw/environment.md` | — | 完成 |
| C0-H02 | 硬件事实 + 我的纸面预测 | 1.5h | 见 [h02-hw-facts-and-prediction.md](h02-hw-facts-and-prediction.md) | C0-R01、R02 | 未开始 |
| C0-H03 | 规模扫描测量：n = 256 → 2048 | 2h | 见 [h03-scale-sweep.md](h03-scale-sweep.md) | C0-R02 | 未开始 |
| C0-H04 | 向量化报告与 SIMD 测量 | 1.5h | 见 [h04-simd.md](h04-simd.md) | C0-R03 | 未开始 |
| C0-H05 | 总结与 v1.0 | 3h | 见 [h05-summary-release.md](h05-summary-release.md) | — | 未开始 |

## 支线（现成实践课程，选做，不计入进度；做完对 Muse 说"做完 <ID>"；状态：未开始 / 完成）
| ID | 实践课程 | 做哪些 | 配合主线（读完 / 做完之后） | 预计用时 | 状态 |
|---|---|---|---|---:|---|
| C0-S01 | [Performance Ninja](https://github.com/dendibakh/perf-ninja) | 实验 [memory_bound/loop_interchange_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/memory_bound/loop_interchange_1)（循环交换，先看实验页顶部的视频） | C0-R01 | 1h | 未开始 |
| C0-S02 | Performance Ninja（同上） | 实验 [memory_bound/loop_tiling_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/memory_bound/loop_tiling_1)（分块） | C0-R02 | 1.5h | 未开始 |
| C0-S03 | Performance Ninja（同上） | 实验 [core_bound/vectorization_1](https://github.com/dendibakh/perf-ninja/tree/main/labs/core_bound/vectorization_1)（向量化） | C0-R03 | 1.5h | 未开始 |

第一次做之前，对 Muse 说"准备环境 Performance Ninja"：按课程的 [Windows 快速开始](https://github.com/dendibakh/perf-ninja/blob/main/QuickstartWindows.md) 装好 CMake 和 Clang，能跑 `validateLab` 与 `benchmarkLab`（约 0.5h，不计入进度）。

## 加分（必做全部完成后才碰）
| ID | 任务 | 计划用时 |
|---|---|---:|
| C0-B01 | 用 Intel VTune 对 n = 2048 的 ikj 和分块各采一次缓存未命中，把 H03 的解释从"推断"升级为"测量" | 3h |
| C0-B02 | 分块 + 打包（packing），看能否超过 ikj（思路见 C0-R04 的 Designing the Kernel、Blocking 两节） | 4h |

## 砍线顺序
1. C0-B02
2. C0-B01
3. （仍然超时）C0-P02 不手写 intrinsics，C0-H04 只做自动向量化对比，在总结中写明

## 收尾定义
2026-12-06：三栏必做全部完成 → 打 `v1.0` → 冻结 `start/`（见 C0-H05），本仓库只保留成果 → 进入 ai-infra-plan。没完成的必做项在总结的"边界与未完成"一节写明，同样收尾。
