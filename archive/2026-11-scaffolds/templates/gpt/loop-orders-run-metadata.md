# 循环顺序 C1 运行记录（模板）

它是什么：零醛在自己的 Windows 电脑上跑 `scripts/run-loop-orders-c1.ps1` 之后，抄进新结果目录的 `metadata.md` 底稿。
谁写的：脚手架由 AI 维护。本页没有实测数字。
怎么用：复制到 `experiments/results/YYYY-MM-DD-loop-orders-c1-windows/metadata.md`，再填下面的空位。不要覆盖已有结果目录。
限制：云端或 Linux 上看到的毫秒不填到这里。未实现的 kernel 没有样本。不含 SCALE-Sim。

- 日期：
- 系统：
- CPU：
- 编译器与选项：
- 源码文件：`templates/gpt/exp-loop-orders.cpp`（编译前把四个 `TODO(零醛)` kernel 写成我自己的版本，或记下当时仍未实现的名字）
- 命令：
- C1：n 为 128 与 256；六种顺序；每种 10 个顺序进程；亲和性 mask 1。逻辑处理器 0 是否为 P-core：待零醛确认。
- 电源计划（`powercfg /getactivescheme` 的原文）：
- 是否改过电源计划：
- 进程内 `--runs`：
- 计时范围：含该 kernel 的 C 清零和乘法；初始化与 int64 对照在计时外。
- 未实现因而没有样本的顺序：
- 结果文件：
- 我没有做的：SCALE-Sim；把本模板里的空位写成数字。
