# cpp-matmul-performance

Matrix multiplication in C++: loop order, blocking and SIMD, measured on a laptop CPU.

用 C++ 矩阵乘法学习访存与局部性：循环顺序、分块、SIMD，每一步都有正确性校验和计时数据。

## 学习进度
<!-- progress:start -->
```
当前战役 C0 cpp-matmul 收尾 v1.0 · 截止 2026-12-06
  阅读 ░░░░░░░░░░░░░░░░░░░░   0%  0/4 项完成 · 已用 0h / 计划 5h
  编程 ░░░░░░░░░░░░░░░░░░░░   0%  0/2 项完成 · 已用 0h / 计划 5h
  动手 █░░░░░░░░░░░░░░░░░░░   6%  1/5 项完成 · 已用 0.5h / 计划 8.5h
当前阅读 C0-R01 Algorithmica 第 8 章 External Memory（主教材全程只用这一本）
当前编程任务 C0-P01 大矩阵版 ikj 与分块（等 H02 的预测提交后开始）
当前动手任务 C0-H02 硬件事实 + 我的纸面预测（读完 R01 后）
最近学习 2026-11-25
```
<!-- progress:end -->

## 我怎么开始学习
| 我想 | 怎么做 |
|---|---|
| 学理论（主线）+ 做练习（支线，选做） | 读 Algorithmica 的当前章节（不和 AI 对话，不考核）；配套练习在 Performance Ninja 上做；读完 / 做完对 Muse 说"读完 <R编号>" / "做完 <S编号>" |
| 写代码（kernel、正确性测试） | GPT："开始写代码" … "结束"（结束时自动归档） |
| 跑测量、看向量化报告、作图 | Muse："做 <H任务ID>" |

分工见 [AGENTS.md](AGENTS.md)，路线见 [ROADMAP](start/ROADMAP.md)，当前位置见 [STATE](start/STATE.md)；学习过程（时间线、日志、笔记）见 [journey/](journey/README.md)。

## 学习成果
<!-- results:start -->
| # | 做了什么 | 结论 | 目录 |
|---|---|---|---|
| 01 | 循环顺序 ijk vs ikj | `ikj` 快 6.85–9.38 倍（n = 64–256，中位数） | [experiments/01-loop-order](results/experiments/01-loop-order/) |
| 02 | 分块 b = 16 / 32 / 64 | n = 128、256 时分块比 `ikj` 慢 11–60%；n = 64、b = 64 基本持平 | [experiments/02-blocking](results/experiments/02-blocking/) |
| 03 | 编译选项 /O2 vs /Od | `/O2` 下分块仍慢于 `ikj`；`/Od` 下 n = 256 的 `ikj` 从约 1.3 ms 变为 22.8–24.8 ms | [experiments/03-compiler-flags](results/experiments/03-compiler-flags/) |
<!-- results:end -->

测量环境：Windows，Intel Core Ultra 7 255HX，Release x64；数字只代表这台机器。复现：`bash results/tests/smoke.sh`（需 g++，只验证编译和正确性、不计时）；详见 [results/](results/README.md)。相关仓库：[AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning)（CUDA 路线）· [IC-Design-Learning](https://github.com/lingquan11/IC-Design-Learning)（数字 IC 路线） · 工作流：[learning-workflow](https://github.com/lingquan11/learning-workflow)（三个仓库共用的规则、提示词和进度脚本）
