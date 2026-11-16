# 架构图（给新开窗口的Agent读）

本文由Muse维护，供任何Agent在无上下文时快速理解本仓库：只读本文加目标模块，就应能说清结构并定位问题。本文不是零醛本人的学习记录；进度真相以`PROJECT_STATE.md`为准，本文与代码实际不符时以代码为准并顺手修正本文。

## 一句话定位

C++矩阵乘法性能实验仓：`src/`是现行成品，`experiments/`存已归档的实测数据与报告（只读），`templates/gpt/`是待零醛本人实现的脚手架，`tests/`只验正确性、不产性能数据。

## 模块地图

| 模块 | 职责 | 依赖方向 |
|---|---|---|
| `src/` | 现行成品程序，一实现一文件（Day1/2/4/5/7共5个`.cpp`）。Day7文件是标注来源的GPT整合版 | 不依赖其他模块；被`tests/day07-correctness.cpp`以`#include "../src/day07-matrix-blocking.cpp"`直接包含 |
| `code_history/` | 各日代码演变快照，按原样冻结，与`src/`并存、哈希不同不合并 | 叶子：只被引用说明，不被编译、不被任何代码依赖 |
| `templates/gpt/` | AI维护的脚手架与模板（规模扫描、循环顺序、LDA、SIMD、PMC探针等）；标`TODO(零醛)`处由零醛本人实现，未实现时打印「未实现」并以非零码退出 | 被`tests/`中对应脚手架测试引用；**不得**移入`src/`冒充成品 |
| `tests/` | 正确性层：`smoke.sh`编译`src/`全部文件、验Day1/2输出、跑Day7正确性（392组合＋非法参数以退出码2被拒）；另有脚手架正确性测试（未实现时SKIP、多数未接入`smoke.sh`）与Python格式/夹具测试 | 依赖`src/`与`templates/gpt/`；不写CSV、不设性能阈值 |
| `scripts/` | 实验驱动与汇总：Windows计时启动脚本（`.bat`/`.ps1`）、Python汇总与绘图（中位数、规模扫描、分块表、数据流账本、汇编关键词扫描等） | 读`experiments/results/`的归档CSV产出汇总；不改原始数据 |
| `experiments/` | 数据层：`results/`归档原始CSV与元数据、`reports/`实验报告、`figures/`可重建图表、`summarize_medians.py`只读重算中位数 | 叶子：数据只增不改，历史数字不倒填、不覆盖；换计时口径即新实验、新目录 |
| `notes/` | 零醛第一人称学习笔记与记录（含`supplements/`补记） | 叶子：只记录，不被代码依赖；视角与口吻不得改写 |
| `plan/` | Day9起学习计划任务卡（段一已齐，`seg1-scale-sweep.md`），供tutor型AI按卡带练 | 读`PROJECT_STATE.md`定进度；不冒充学习记录 |
| `benchmarks/` | 预留的正式计时驱动目录，现只有说明、无可跑脚本 | 无依赖；云端计时脚手架已移除、不恢复 |
| `.github/workflows/compile.yml` | CI：跑`tests/smoke.sh`＋只读中位数检查（`summarize_medians.py`前30行） | 依赖`tests/`与`experiments/`，不跑任何计时入口 |
| 根目录 | `README.md`展示门面；`PROJECT_STATE.md`唯一进度真相 | 先读`PROJECT_STATE.md`，再读本文 |

## 依赖规则（改动时守住）

1. 依赖只能单向向下：`tests→src/templates`、`scripts→experiments数据`、`CI→tests/experiments`。`src/`不反向依赖测试或脚本，归档数据不反向依赖任何代码。
2. 无循环依赖：`code_history/`、`experiments/`、`notes/`三处是叶子，只存不改（数据与快照的修改走新增，不走覆盖）。
3. 脚手架与成品物理隔离：`templates/gpt/`里的`TODO(零醛)`实现完成并经零醛确认后，才谈移入`src/`；AI不得代填这些实现。

## 按问题定位（要改/要查什么，先读哪）

| 问题 | 先读 |
|---|---|
| 现行实现怎么算、改算法 | `src/README.md`＋对应`src/dayNN-*.cpp`，来源疑问查`notes/dayNN-code-materials.md` |
| 正确性挂了、CI红了 | `tests/smoke.sh`＋`tests/day07-correctness.cpp`（它包含`src/day07-matrix-blocking.cpp`） |
| 某个数（中位数/倍率）从哪来 | `experiments/summarize_medians.py`重算＋`experiments/results/`对应日期目录的原始CSV与`metadata.md` |
| 脚手架某项能不能跑 | `templates/gpt/README.md`＋对应`tests/*-correctness.cpp`：未实现应为SKIP或非零退出，不是故障 |
| 现在做到哪、下一步学什么 | `PROJECT_STATE.md`（进度）＋`plan/README.md`（任务卡入口） |
| 历史某天代码长什么样 | `code_history/dayNN/`对应快照，不动`src/` |

## 架构健康自查（仓库优化每轮问一遍）

- 可读性：新Agent只读本文＋目标模块，能否说清结构？
- 循环依赖：本次改动有没有让叶子目录被代码依赖、或让`src/`反向依赖？
- 复杂度：有没有新增跨模块耦合，或把脚手架混入成品？
- 架构健康：本文模块地图与实际目录是否仍一致？不一致先修本文。
