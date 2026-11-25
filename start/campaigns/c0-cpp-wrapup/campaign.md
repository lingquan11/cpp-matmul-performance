# 战役 C0：cpp-matmul 收尾 v1.0

- **周期**：2026-11-23 → 2026-12-06（截止日不延期）
- **预算**：每周 8–10h，合计约 17.5h（阅读 4h + 编程 5h + 动手 8.5h）；三区重构 C0-T01 已于 2026-11-25 通过，不再列入
- **唯一交付物**：`v1.0` Release。`results/README.md` 用一张表回答"循环顺序、分块、SIMD 在不同规模下各带来什么"，并附 `results/summary.md`
- **统一约定**：元素类型 `int`；计时只在我说"开始测量"时运行；每个配置预热 1 次、正式 5 次取中位数；接通电源、"最佳性能"模式、绑定到一个 P 核；吞吐量用 GOPS = 2·n³ / 耗时（int 运算，不叫 GFLOPS）
- **推荐顺序**：R01 → H02（预测先提交）→ P01 → H03 → R02 → P02 → H04 → H05
- **下一个战役**：[AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning) 的 C1 CUDA SGEMM（2026-12-07 起）

## 阅读（我自己读，不和 AI 对话；读完对 Muse 说"读完 <ID>"；状态：未读 / 在读 / 读完）
| ID | 长文 | 读哪些部分 | 预计用时 | 读完应该能回答（自测，不考核） | 状态 |
|---|---|---|---:|---|---|
| C0-R01 | Bryant & O'Hallaron, *Computer Systems: A Programmer's Perspective*（第 3 版）第 6 章 | Locality、Cache Memories、Writing Cache-Friendly Code、Putting It Together 几节；加 Web Aside [MEM:BLOCKING](https://csapp.cs.cmu.edu/public/waside/waside-blocking.pdf) | 2.5h | ijk 和 ikj 的内层循环各以什么步长访问三个矩阵？分块想提高哪种局部性、代价是什么？存储器山说明了什么？ | 未读 |
| C0-R02 | Sergey Slotin, *Algorithms for Modern Hardware*：[Matrix Multiplication](https://en.algorithmica.org/hpc/algorithms/matmul/) | 全文（Baseline 到 Blocking） | 1.5h | 转置和向量化各解决了什么问题？为什么寄存器复用的微内核比单纯分块更关键？这能不能解释我的分块没有变快？ | 未读 |

## 编程（GPT 带写，只看机器验收；状态：未开始 / 进行中 / 通过）
| ID | 任务 | 计划用时 | 验收要点（展开任务卡时不得弱于此） | 先读 | 依赖动手 | 状态 |
|---|---|---:|---|---|---|---|
| C0-P01 | 大矩阵版 ikj 与分块 | 2.5h | 见 [p01-large-matrix.md](p01-large-matrix.md) | C0-R01 | C0-H01 | 未开始 |
| C0-P02 | 手写 AVX2 版 ikj | 2.5h | 见 [p02-avx2.md](p02-avx2.md) | C0-R02 | C0-H01 | 未开始 |

## 动手（Muse 负责，只看机器验收；状态：未开始 / 进行中 / 完成）
| ID | 任务 | 计划用时 | 验收要点 | 先读 | 状态 |
|---|---|---:|---|---|---|
| C0-H01 | 环境：Coreinfo、MSVC 命令行、Python + matplotlib | 0.5h | `coreinfo64 -c -l` 有输出；`cl` 能编译 `results/src/blocking.cpp`（须加 `/utf-8`）；作图脚本能运行；记录见 `results/hw/environment.md` | — | 完成 |
| C0-H02 | 硬件事实 + 我的纸面预测 | 1.5h | 见 [h02-hw-facts-and-prediction.md](h02-hw-facts-and-prediction.md) | C0-R01 | 未开始 |
| C0-H03 | 规模扫描测量：n = 256 → 2048 | 2h | 见 [h03-scale-sweep.md](h03-scale-sweep.md) | C0-R01 | 未开始 |
| C0-H04 | 向量化报告与 SIMD 测量 | 1.5h | 见 [h04-simd.md](h04-simd.md) | C0-R02 | 未开始 |
| C0-H05 | 总结与 v1.0 | 3h | 见 [h05-summary-release.md](h05-summary-release.md) | — | 未开始 |

## 加分（必做全部完成后才碰）
| ID | 任务 | 计划用时 |
|---|---|---:|
| C0-B01 | 用 Intel VTune 对 n = 2048 的 ikj 和分块各采一次缓存未命中，把 H03 的解释从"推断"升级为"测量" | 3h |
| C0-B02 | 分块 + 打包（packing），看能否超过 ikj（先读 Goto & van de Geijn, [Anatomy of High-Performance Matrix Multiplication](https://www.cs.utexas.edu/~flame/pubs/GotoTOMS_revision.pdf) 前 4 节） | 4h |

## 砍线顺序
1. C0-B02
2. C0-B01
3. （仍然超时）C0-P02 不手写 intrinsics，C0-H04 只做自动向量化对比，在总结中写明

## 收尾定义
2026-12-06：三栏必做全部完成 → 打 `v1.0` → 冻结 `start/`（见 C0-H05），本仓库只保留成果 → 进入 AI-Infra-Learning 的 C1。没完成的必做项在总结的"边界与未完成"一节写明，同样收尾。
