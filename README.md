# cpp-matmul-performance

Matrix multiplication in C++: loop order, blocking and SIMD, measured on a laptop CPU.

用 C++ 矩阵乘法学习访存与局部性：循环顺序、分块、SIMD，每一步都有正确性校验和计时数据。

## 学习进度
<!-- progress:start -->
```
当前战役 C0 cpp-matmul 收尾 v1.0
  ██░░░░░░░░░░░░░░░░░░  10%  1/6 个必做任务通过 · 已用 0.1h / 计划 14.5h · 截止 2026-12-06
环境准备 0/1 就绪（与 Muse 协作，不计入学习进度）· 已用 0h
当前任务 C0-T02 硬件事实 + 纸面预测
最近学习 2026-11-25
```
<!-- progress:end -->

## 我怎么开始学习
| 我想 | 对谁说 |
|---|---|
| 学习（结束时自动归档、更新进度） | GPT / Codex："开始学习" … "结束" |
| 考核一个做完的任务 | Gemini："考核 <任务ID>" |
| 装环境、配工具 | Muse："准备环境" |

分工见 [AGENTS.md](AGENTS.md)，路线见 [start/ROADMAP.md](start/ROADMAP.md)。
· [当前位置与下一步](start/STATE.md) 

## 学习过程
[journey/](journey/README.md)：时间线、每次学习的日志、按主题整理的笔记、考核记录。

## 学习成果
<!-- results:start -->
| # | 做了什么 | 结论 | 目录 |
|---|---|---|---|
| 01 | 循环顺序 ijk vs ikj | `ikj` 快 6.85–9.38 倍（n = 64–256，中位数） | [experiments/01-loop-order](results/experiments/01-loop-order/) |
| 02 | 分块 b = 16 / 32 / 64 | n = 128、256 时分块比 `ikj` 慢 11–60%；n = 64、b = 64 基本持平 | [experiments/02-blocking](results/experiments/02-blocking/) |
| 03 | 编译选项 /O2 vs /Od | `/O2` 下分块仍慢于 `ikj`；`/Od` 下 n = 256 的 `ikj` 从约 1.3 ms 变为 22.8–24.8 ms | [experiments/03-compiler-flags](results/experiments/03-compiler-flags/) |
<!-- results:end -->

复现：`bash results/tests/smoke.sh`；详见 [results/](results/README.md)。相关仓库：[AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning)（CUDA 路线）· [IC-Design-Learning](https://github.com/lingquan11/IC-Design-Learning)（数字 IC 路线） · 工作流：[learning-workflow](https://github.com/lingquan11/learning-workflow)（三个仓库共用的规则、提示词和进度脚本）
