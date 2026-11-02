# 2026-11-02 软件脉动阵列周期模型

软件模型，非硬件实测。运行耗时不代表性能。

- 这一天是脚手架写入仓库的日期，不是测量日期。我没有运行模型，也没有硬件计数。
- 本目录没有 `model.csv`。等我实现 `step_one_cycle`、`inject_skewed_inputs`、`run_tile` 之后自己跑，再把 CSV 放进来。不要把云端、Linux 虚拟机或 CI 里的耗时写进这个文件或 CSV。
- 计时实验：无新进展。没有新的性能样本。
- 程序不在 `src/`。脚手架是 [`templates/gpt/systolic-os-model.cpp`](../../../templates/gpt/systolic-os-model.cpp)，编译用 [`scripts/build-systolic-os-model.sh`](../../../scripts/build-systolic-os-model.sh)。
- 解析式 `expected_cycles(n,P) = (⌈n/P⌉)² × (n + 2P − 2)`，以及列出的 4 拍、24 拍、4992 拍、50%、约 66.7%，都待零醛手算核对。它们不是本目录的运行结果。
- 正式报告还没有。手算空表在 [`notes/dayNN-systolic-notes.md`](../../../notes/dayNN-systolic-notes.md)。日程序号未定。
