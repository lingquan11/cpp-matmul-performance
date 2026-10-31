# cpp-matmul-performance

**问题。** 我用可复现的 C++ 计时，看朴素矩阵乘法在规模变大以后，耗时为什么不跟着运算次数走，从而理解数据复用为什么对 NPU 方向重要。

**读者。** 我是东南大学电子信息专业大一，这是我的学习路径。也给在学 C++ 性能、或者刚开始碰 NPU 基础的同学看。

**做到哪。** 阶段 A 已完成（Day 1–2；Day 3 零矩阵等）。阶段 B 进行中：本地计时和循环顺序对比已阶段性完成（Day 4、Day 5），编译优化还没有单独对比。阶段 C 已启动：分块有了 75 条和边界测试，正式报告进行中，分块还没有普遍快过 `ikj`。阶段 D 未开始。计时复现：未验证。

> **English:** Undergraduate portfolio repo (SEU, EE): reproducible C++ matrix-multiply experiments on CPU—correctness first, then timing, loop order (`ijk` vs `ikj`), and cache blocking—with archived CSV on Windows. Systolic-array / SCALE-Sim (stage D) and real NPU runs are not started. Progress: [`PROJECT_STATE.md`](PROJECT_STATE.md).

## 最值得先看

1. **Day 5 循环顺序（先看这个）。** [报告](experiments/reports/2026-10-26-day05-loop-order.md) 和 [30 条结果](experiments/results/2026-10-26-day05-loop-order-local/README.md)。同一次运行里，64 / 128 / 256 的 `ijk` 中位数相对 `ikj` 是 6.85× / 8.45× / 9.38×。程序在 [`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp)。
2. **Day 7 分块。** [报告](experiments/reports/2026-10-27-day07-blocking.md)。`blocked_64` 在三种分块里最快，`n=128` 和 `n=256` 仍慢于 `ikj`。只看耗时不能证明缓存未命中。
3. **进度。** [`PROJECT_STATE.md`](PROJECT_STATE.md)。和本页不一致时，以它为准。
4. **现行程序。** [`src/README.md`](src/README.md)。Day 5 双版本是我提交的；[`src/day07-matrix-blocking.cpp`](src/day07-matrix-blocking.cpp) 是 GPT 整合版，不是我本机逐字源码。

## 与 NPU 的关系

1. **CPU 上的矩阵乘法。** 这是我目前在做的。阶段 A 的实现做完了。阶段 B 有本地计时和循环顺序。阶段 C 的分块开始了，还没有普遍快过 `ikj`。上面的毫秒都是 CPU 计时。
2. **软件上的脉动阵列和数据流建模。** 还没做。阶段 D 未开始。以后如果写下软件仿真时间，要单独标明，不能写成真实 NPU 的性能。
3. **真实 NPU 硬件。** 还没做。仓库里没有 NPU 上的测量。

## 学习路径一览（约 3 分钟）

下面按「基线 → 循环顺序 → 分块 → 脉动阵列」串起来；数字只引用已归档 CSV/报告，不在这里重跑计时。

| 阶段 | 在解决什么问题 | 主要改动 | 怎么验证 | 这轮结论（有出处） | 代价与限制 | 证据入口 |
| --- | --- | --- | --- | --- | --- | --- |
| **基线 · 规模与计时**（Day 4，阶段 B） | 规模变大后，耗时为何不像 O(n³) 那样均匀变长 | 从 2×2 到可输入 `n` 的计时；同机有两份 15 条样本（截图转录 vs 程序导出） | 逐行 PASS；Day 4 截图转录那组完整 benchmark 源码仍缺 | 同阶 `n` 下两份 CSV 数值不同，不合并；128 阶有偏高单次，根因未知 | 单次 `n=4` 只适合认输出，不能当性能结论；计时复现未验证 | [Day 4 结果](experiments/results/2026-10-26-day04-local/README.md)、[记录](experiments/reports/2026-10-26-day04-local.md) |
| **循环顺序**（Day 5） | 只改 `i/j/k` 顺序，中位数差几倍 | 同一次运行交替测 `ijk` 与 `ikj` | 30 条 PASS，逐元素 YES | 64/128/256 阶 `ijk/ikj` 中位数比约 **6.85× / 8.45× / 9.38×**（[Day 5 报告](experiments/reports/2026-10-26-day05-loop-order.md)） | 只属于该程序、该规模、该次 Windows 运行；编译优化未单独对比 | [30 条 CSV](experiments/results/2026-10-26-day05-loop-order-local/README.md)、[`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp) |
| **分块**（Day 6 概念 + Day 7 实测，阶段 C） | 能否用小块数据提高复用、缩短耗时 | `blocked_16/32/64` 与 `ijk`/`ikj` 同口径计时 | 75 条 PASS；`n=70, block=32` 边界测试 PASS | `blocked_64` 最快，但 **128/256 仍慢于 `ikj`**；`n=64` 与 `ikj` 中位数接近、样本范围重叠 | 总耗时**不能**证明 cache miss 变了多少；[`day07-matrix-blocking.cpp`](src/day07-matrix-blocking.cpp) 为 GPT 整合版 | [Day 7 报告](experiments/reports/2026-10-27-day07-blocking.md)、[75 条 CSV](experiments/results/2026-10-27-day07-blocking-local/README.md) |
| **脉动阵列 / SCALE-Sim**（阶段 D） | NPU 方向的数据流与 MAC 阵列 | **未开始** | — | — | 以后软件仿真时间须与真实 NPU 测量分开写 | 路线图见 [`PROJECT_STATE.md`](PROJECT_STATE.md) |

下一阶段主线是 **SIMD**（尚未入库实现）。Day 8 性能诊断理论学习归档见 [PR #21](https://github.com/lingquan11/cpp-matmul-performance/pull/21)（待合并），无新 CSV。

## 3 分钟证据

下面两张表从已有报告抄来，数字和报告一致。想从归档 CSV 再打一遍，在仓库根目录跑：

```bash
python3 experiments/summarize_medians.py
```

脚本只读，不覆盖 CSV，也不重新计时。用法写在 [`experiments/README.md`](experiments/README.md)。计时复现：未验证。机器以 [`PROJECT_STATE.md`](PROJECT_STATE.md) 为准：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。

### Day 5：`ijk` 和 `ikj`

**这张表在回答：** 同一次运行、同一套输入里，只改循环顺序，中位数耗时差几倍？

来源：[Day 5 报告](experiments/reports/2026-10-26-day05-loop-order.md)。每组 5 次，中位数是排序后的第 3 个 `ms_per_multiply`。比值是 `ijk` 中位数除以 `ikj` 中位数，写到两位小数。

| n | ijk 中位数 (ms) | ikj 中位数 (ms) | 中位数耗时比（ijk/ikj） |
| ---: | ---: | ---: | ---: |
| 64 | 0.140724 | 0.020545 | 6.85× |
| 128 | 1.375541 | 0.162732 | 8.45× |
| 256 | 11.333944 | 1.207955 | 9.38× |

**结论。** 这一轮里 `ikj` 的中位数低于 `ijk`。内层 `j` 连续走，`B[k][j]` 和 `C[i][j]` 更顺着一行，和按行存储对得上。6.85× / 8.45× / 9.38× 只属于这次程序、这次规模、这次运行。

### Day 7：分块和 `ikj`

![Day 7 历史样本：分块在 128、256 阶均慢于 ikj；64 阶 blocked_64 与 ikj 的范围重叠。](experiments/figures/day07-speedup.svg)

**这张图回答：分块是否比 `ikj` 更快？** 点为全部样本，横线为观测范围，黑色短线为中位数之比；越靠右越快，虚线 1 为同尺寸 `ikj` 基准。范围不是置信区间。关注 128、256 阶的分块点仍在 1 左侧。[图表说明与重生成](experiments/figures/README.md) · [汇总 CSV](experiments/figures/day07-summary.csv)


**这张表在回答：** 同一轮里，三种分块的中位数有没有普遍快过 `ikj`？

来源：[Day 7 报告](experiments/reports/2026-10-27-day07-blocking.md)。每组 5 个正式样本的中位数，单位毫秒。

| n | ijk | ikj | blocked_16 | blocked_32 | blocked_64 |
|---:|---:|---:|---:|---:|---:|
| 64 | 0.142012 | 0.021578 | 0.030522 | 0.024964 | 0.021118 |
| 128 | 1.162850 | 0.150144 | 0.231649 | 0.194287 | 0.166712 |
| 256 | 10.463860 | 1.154853 | 1.843587 | 1.573395 | 1.317015 |

**结论。** `blocked_64` 是三种分块里最快的；`n=128` 和 `n=256` 时仍然慢于 `ikj`。只看总耗时不能证明缓存未命中变了多少。`n=64` 时 `blocked_64` 和 `ikj` 的中位数大约差 2.1%，五次样本的范围叠在一起，不能只凭中位数说它稳定更快。

## 当前做到哪

每天的记录、缺口和下一步只写在 [`PROJECT_STATE.md`](PROJECT_STATE.md)。这里只留路线图和阶段表。和状态文件冲突时，以状态文件为准。

### 路线图

- [x] **阶段 A · 算法实现**：完整三重循环实现矩阵乘法，用小矩阵手算结果核对正确性，并分析时间复杂度。已完成（Day 1–2；Day 3 零矩阵等）。
- [ ] **阶段 B · 性能测量**：计时方法，以及矩阵规模、循环顺序、编译优化等控制变量。进行中。本地计时和循环顺序对比已阶段性完成（Day 4 规模、计时、截图转录与程序导出；Day 5 `ijk` / `ikj` 30 条验收）。编译优化选项还没有单独对比。计时复现未验证。
- [ ] **阶段 C · 分块优化与实验报告**：分块（tiling），并写出可复现的实验报告。已启动（Day 6 概念；Day 7 分块 75 条与边界测试 PASS）。正式报告进行中。分块尚未普遍快过 `ikj`。
- [ ] **阶段 D · 脉动阵列与 SCALE-Sim**：理解脉动阵列数据流，用 SCALE-Sim 对照前面的实测结果。未开始。

### 阶段成果

| 阶段 | 状态 | 成果 |
| --- | --- | --- |
| A · 算法实现 | 已完成（Day 1–2；Day 3 零矩阵等） | [`src/day01-matrix.cpp`](src/day01-matrix.cpp) 算单个输出元素；[`src/day02-matrix.cpp`](src/day02-matrix.cpp) 是固定 2×2。报告在 [阶段 A](experiments/reports/stage-a-algorithm-implementation.md)。 |
| B · 性能测量 | 进行中；本地计时与循环顺序对比阶段性完成 | Day 4 截图转录与程序导出：[结果](experiments/results/2026-10-26-day04-local/README.md)、[记录](experiments/reports/2026-10-26-day04-local.md)。Day 5 的 30 条：[结果](experiments/results/2026-10-26-day05-loop-order-local/README.md)、[记录](experiments/reports/2026-10-26-day05-loop-order.md)。截图转录那组的完整 benchmark 源码仍缺。计时复现：未验证。 |
| C · 分块优化与实验报告 | 已启动；正式可复现实验报告进行中 | Day 6 概念在 [`notes/day06-notes.md`](notes/day06-notes.md)，没有新测量。Day 7 正式 CSV 75 条全部 PASS，`n=70`、`block_size=32` 边界测试通过：[结果](experiments/results/2026-10-27-day07-blocking-local/README.md)、[分块记录](experiments/reports/2026-10-27-day07-blocking.md)。[`src/day07-matrix-blocking.cpp`](src/day07-matrix-blocking.cpp) 是 GPT 整合版，不是用户本机逐字源码。分块尚未普遍快过 `ikj`。 |
| D · 脉动阵列与 SCALE-Sim | 未开始 | — |

## 怎么跑

### Windows 最小实验与分块正确性

需要支持 C++17 的编译器。下面在 Windows 的 **x64 Native Tools Command Prompt for VS** 中，从仓库根目录运行；已经安装 C++ 工具链的情况下无需额外库：

```bat
chcp 65001 > nul
if not exist build mkdir build
cl /nologo /std:c++17 /O2 /EHsc /utf-8 src\day02-matrix.cpp /Fo:build\day02.obj /Fe:build\day02.exe
build\day02.exe
cl /nologo /std:c++17 /O2 /EHsc /utf-8 src\day04-matmul.cpp /Fo:build\day04.obj /Fe:build\day04.exe
echo 4 | build\day04.exe
```

Day 2 输出两行 `11 10` 和 `19 30`；Day 4 的 `n=4` 应输出 `C[0][0] = 50`。Day 4 的单次耗时适合认识计时范围，**不足以用于性能结论**；可把 `4` 改为 1–256 的整数观察规模变化。若找不到 `cl`，请从 Visual Studio Installer 安装 C++ 桌面开发组件并打开上述开发者命令提示符。

接着验证分块，改变 `n` 或块大小：

```bat
cl /nologo /std:c++17 /O2 /EHsc /utf-8 tests\day07-correctness.cpp /Fo:build\day07-correctness.obj /Fe:build\day07-correctness.exe
build\day07-correctness.exe 70 32
build\day07-correctness.exe 70 64
build\day07-correctness.exe
```

两个单例各输出 `PASS: 12 algorithm/input cases`；最后一条运行 392 个组合。这个入口只做正确性比较，不计时、不写 CSV。Windows/MSVC 下上述最小流程曾在 2026-10-29 核对（见[维护记录](notes/maintenance-2026-10-29.md)）；**本次维护未在 Windows 上复核**。性能 benchmark 的规模与分块大小写在源码常量里（见 [tests/README.md](tests/README.md)「改 matrix 规模与分块大小」）；重跑会写本地 CSV，不能覆盖归档文件。

只想比较已有性能结果，可以运行 `python scripts/summarize_day07.py`（需要 Python 和 Matplotlib），由原始样本重新生成首页图表；这不执行矩阵乘法，也不产生新的性能证据。

Day 7 的固定容量数组为 256×256，逻辑规模小于 256 时物理行跨度仍为 256 个 `int`。整合版使用整数输入与乘加，修改输入范围或扩大容量时需重新检查溢出；不要将它当成浮点误差测试。实验记录、生成图表和独立正确性检查见下方入口。

### Linux 构建与只读中位数复算

前提：`g++` 支持 C++17。冒烟脚本调用的就是 `g++ -std=c++17 -O2`。

在仓库根目录：

```bash
bash tests/smoke.sh
```

CI 做的是同一件事。[`.github/workflows/compile.yml`](.github/workflows/compile.yml) 在 `ubuntu-latest` 上执行：

```bash
sudo apt-get update && sudo apt-get install -y g++
bash tests/smoke.sh
```

脚本会编译 [`src/`](src/README.md) 下现存的 `.cpp`，然后运行两份有固定输出的程序和独立的 Day 7 正确性测试：

- Day 1（[`src/day01-matrix.cpp`](src/day01-matrix.cpp)）：检查输出 `11`
- Day 2（[`src/day02-matrix.cpp`](src/day02-matrix.cpp)）：检查输出 `11 10` 和 `19 30`

Day 7 另由 [`tests/day07-correctness.cpp`](tests/day07-correctness.cpp) 调用计算函数，检查 392 个组合和非法参数。Day 4、Day 5、Day 7 的 benchmark main 只编译，不运行；不输出性能 CSV。历史计时复现仍未验证。

非交互跑 Day 4 最小正确性（**不要把 Linux 上打印的毫秒写进正式结论**）：

```bash
mkdir -p build
g++ -std=c++17 -O2 -o build/day04-matmul src/day04-matmul.cpp
printf '4\n' | build/day04-matmul   # 期望含 C[0][0] = 50
```

想看循环顺序怎么比，读 [`src/day05-ijk-ikj-benchmark.cpp`](src/day05-ijk-ikj-benchmark.cpp)，再对照已经归档的 [Day 5 CSV](experiments/results/2026-10-26-day05-loop-order-local/day05-loop-order-program-export.csv) 和 [Day 5 报告](experiments/reports/2026-10-26-day05-loop-order.md)。这里不把重跑计时写进 CI。

### 常见问题

| 现象 | 说明 |
| --- | --- |
| `cl` / `g++` 找不到 | Windows 用 **x64 Native Tools Command Prompt**；Linux 安装 `g++` 并在仓库根目录执行命令 |
| Day 4 卡住要输入 | 在提示处输入 1–256 的整数，或用 `printf '4\n' \| …` 管道输入 |
| 运行 Day 5/7 benchmark 多了 CSV | 程序会在**当前工作目录**写 `matmul_benchmark_day5_loop_order.csv` 或 `matmul_benchmark_day7_blocking.csv`；这是新样本，勿提交、勿覆盖 `experiments/results/` |
| 中位数和 README 不一致 | 用 `python3 experiments/summarize_medians.py` 只读归档 CSV；若仍不一致，查是否误用了本地新 CSV |
| CI 绿但本地 Windows 计时对不上 | CI 是 Linux `g++` 冒烟 + 正确性，**不能**代替 Visual Studio 2026 Release/x64 上的历史计时 |

我的正式计时在 Windows 上，Visual Studio 2026 18.11.1，Release / x64。上面的冒烟是 `g++` 编译检查，不能代替那次计时。MSVC toolset 独立版本号未记录。`benchmarks/` 只是占位，仓库里没有云端可跑的性能基准。

只想复核报告里的中位数、又不想重新计时：

```bash
python3 experiments/summarize_medians.py
```

需要 Python 3 标准库。说明在 [`experiments/README.md`](experiments/README.md)。

## 结果在哪

最重要的两张中位数表在上面的「3 分钟证据」。报告从 [`experiments/reports/`](experiments/reports/README.md) 进入。

Day 4 的截图转录和程序导出各 15 条，在 [`experiments/results/2026-10-26-day04-local/`](experiments/results/2026-10-26-day04-local/README.md)，短记录在 [Day 4 本地实验记录](experiments/reports/2026-10-26-day04-local.md)。两份数值不同，都不并进 Day 5。截图转录那组的完整 benchmark 源码仍缺。

Day 5 的 30 条在 [`experiments/results/2026-10-26-day05-loop-order-local/`](experiments/results/2026-10-26-day05-loop-order-local/README.md)。Day 7 的 75 条在 [`experiments/results/2026-10-27-day07-blocking-local/`](experiments/results/2026-10-27-day07-blocking-local/README.md)。虚拟机或大规模扫描的结果不在库里。材料缺口：待用户提供。

## 笔记在哪

学习笔记在 [`notes/`](notes/README.md)：

- 理解笔记 `dayNN-notes.md`，Day 5 在 [`notes/day05-notes.md`](notes/day05-notes.md)
- 每日归档 `dayNN-record.md`，Day 5 在 [`notes/day05-record.md`](notes/day05-record.md)
- 每天的索引 [`notes/daily-task-records.md`](notes/daily-task-records.md)

## 目录

| 路径 | 里面是什么 |
| --- | --- |
| [`src/`](src/README.md) | 这一阶段的现行文件。Day 7 的 `day07-matrix-blocking.cpp` 是 GPT 整合版，不是用户逐字源码。文件名用连字符 |
| [`templates/gpt/`](templates/gpt/README.md) | GPT 模板。不是我独立写的，也不放在 `src/` |
| [`code_history/`](code_history/README.md) | 各日代码演变快照。和 `src/` 并存，哈希不同不合并。快照文件名保持原样 |
| [`benchmarks/`](benchmarks/README.md) | 占位。正式驱动等我本地测稳后再放。没有云端可跑基准 |
| [`tests/`](tests/README.md) | 编译 `src/`，检查 Day 1/2 已知输出和 Day 7 的独立正确性。不含性能测试 |
| [`.github/workflows/compile.yml`](.github/workflows/compile.yml) | 做上面的编译与正确性检查。不是计时 CI |
| [`experiments/`](experiments/README.md) | 实验入口。分析在 `experiments/reports/`，原始数据在 `experiments/results/`。`summarize_medians.py` 只读 Day 5、Day 7 的程序导出 CSV，再打中位数 |
| [`experiments/reports/`](experiments/reports/README.md) | 实验记录和阶段报告 |
| [`experiments/results/`](experiments/results/README.md) | 实测记录。Day 4 的转录和程序导出分文件放在同一目录；Day 5 是 30 条程序导出；Day 7 是 75 条分块程序导出 |
| [`notes/`](notes/README.md) | 每日学习记录和理解笔记 |
| [`PROJECT_STATE.md`](PROJECT_STATE.md) | 当前状态。和本页不一致时，以它为准 |

## 参与与下一步

采用 [MIT 许可证](LICENSE)。复现问题可以开 issue，注明系统、编译器、命令与输出。新人小任务：先预测 `n=70` 配块 32、64 时的尾块边界，再运行正确性检查；解释为什么都算对不代表一样快。2026-10-29 Windows 核验见[维护记录](notes/maintenance-2026-10-29.md)；2026-10-31 维护在 Linux VM 复核 `bash tests/smoke.sh` 与 `python3 experiments/summarize_medians.py`（无新性能数字）。
