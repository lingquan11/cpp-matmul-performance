# 实验环境补充记录

本文件是 2026-10-27 材料补缴，不是新的一天，也不改写已入库的 CSV、截图、实验报告或结果目录原文。

## 沿用此前已提交的记录

这些字段在本次补丁之前已经写进仓库，继续有效。不改成「未记录」，也不再向用户索要。

- 操作系统：Windows。
- CPU：Intel Core Ultra 7 255HX。任务管理器截图在 [`experiments/results/2026-10-26-day04-local/cpu-intel-core-ultra-7-255hx.png`](../../experiments/results/2026-10-26-day04-local/cpu-intel-core-ultra-7-255hx.png)，处理器一栏为 Intel(R) Core(TM) Ultra 7 255HX。
- 正式构建/运行配置：Release / x64。配置截图在 [`experiments/results/2026-10-26-day05-loop-order-local/visual-studio-release-x64.png`](../../experiments/results/2026-10-26-day05-loop-order-local/visual-studio-release-x64.png)。

## 本次用户确认后写入的信息

- Day 4、Day 5、Day 6、Day 7 的本地实验一直使用同一台电脑和同一硬件环境；期间没有更换实验机器或硬件。因此 Day 6、Day 7 沿用上面已经提交的 Windows 与 Intel Core Ultra 7 255HX，以及 Release / x64。
- 开发环境：Visual Studio 2026，版本 **18.11.1**。这是用户提供的 IDE 版本。此前笔记里的「18.x」收窄为这个 IDE 版本号。

## 冲突处理

补丁原文把「CPU 具体型号」写成未记录。仓库里此前已经提交了 Intel Core Ultra 7 255HX，并有截图。用户裁定：CPU、硬件参数及其他已经提交的运行环境参数沿用旧记录，不要改成「未记录」。本文件按这个裁定写。

## 仍未记录

- MSVC C++ 编译器 / toolset 的独立版本号：未记录。
- Windows 完整版本号、内存容量、BIOS、电源模式、测试时后台进程状态：此前材料没有给出具体值，仍是未记录。本次不补造，也不再索要。

Visual Studio IDE 版本与 MSVC 编译器/toolset 版本不是同一层信息。`18.11.1` 只记 IDE。不把 IDE 版本推断成 toolset 版本。

Day 7 结果目录里的 `metadata.md` 在归档当时写过「环境：未记录」。那份原文本次不改。现行环境事实以本文件和 [`PROJECT_STATE.md`](../../PROJECT_STATE.md) 为准：同机、Windows、Intel Core Ultra 7 255HX、Visual Studio 2026 18.11.1、Release / x64；MSVC toolset 独立版本号未记录。

## 解释实验数据时的约束

各天实验运行环境保持一致，可以做同机对比。这不代表每次运行时的瞬时系统状态完全相同。后台进程、操作系统调度、CPU 频率或功耗状态、温度和其他短时因素仍可能造成样本波动。

不得根据「环境一直相同」推断出没有任何系统噪声。

## Day 8 环境延续

Day 8 只学性能分析和实验设计，没有跑实验。用户确认 CPU 型号和硬件参数与此前各日不变，沿用上面的 Intel Core Ultra 7 255HX；这次没有重新测量或重新确认型号。编译环境沿用 Visual Studio 2026 18.11.1。MSVC toolset 独立版本号仍未记录。

以后真做 perf 或硬件计数器实验时，要另外记：CPU 拓扑、系统版本、编译器和优化配置、线程亲和性、预热、重复次数、计数器事件和统计区间。
