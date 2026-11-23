# cpp-matmul-performance

Matrix multiplication in C++: loop order, blocking and SIMD, measured on a laptop CPU.

用 C++ 矩阵乘法学习访存与局部性：循环顺序、分块、SIMD，每一步都有正确性校验和计时数据。

## 我怎么开始学习
1. 打开 [start/STATE.md](start/STATE.md)：当前战役、当前任务、下一步
2. 打开它指向的任务卡
3. 把 [start/prompts/tutor.md](start/prompts/tutor.md) + STATE + 任务卡贴给导师 AI，开始学习
4. 学完用 [start/prompts/archiver.md](start/prompts/archiver.md) 归档；任务做完用 `examiner.md` 考核

路线见 [start/ROADMAP.md](start/ROADMAP.md)，规则见 [start/RULES.md](start/RULES.md)。

## 学习过程
[journey/](journey/README.md)：时间线、每次学习的日志、按主题整理的笔记。

## 学习成果
| 做了什么 | 结论 | 在哪里 |
|---|---|---|
| 循环顺序 ijk vs ikj | `ikj` 快 6.85–9.38 倍（n = 64–256） | [results/01](results/experiments/01-loop-order/) |
| 分块 b = 16 / 32 / 64 | n ≤ 256 时没有加速，n = 128、256 慢 11–60% | [results/02](results/experiments/02-blocking/) |
| 编译选项 /O2 vs /Od | `/O2` 下分块仍慢于 `ikj` | [results/03](results/experiments/03-compiler-flags/) |

复现：`bash results/tests/smoke.sh`（编译全部程序并运行正确性测试）。详见 [results/](results/README.md)。

相关仓库：[AI-Infra-Learning](https://github.com/lingquan11/AI-Infra-Learning)（CUDA 路线）· [IC-Design-Learning](https://github.com/lingquan11/IC-Design-Learning)（数字 IC 路线）
