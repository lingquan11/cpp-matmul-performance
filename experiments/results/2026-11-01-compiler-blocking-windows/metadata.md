# 2026-11-01 编译选项与分块复测元数据

- 执行：Codex 根据用户请求在本机运行；不是学生独立完成的测量。
- 系统：Windows 10.0.26200.9711；CPU：Intel Core Ultra 7 255HX（注册表）。
- 编译器：MSVC 19.51.36260 x64；toolset 14.51.36231；Visual Studio 2026 18.11.1。
- 源码：[`src/day07-matrix-blocking.cpp`](../../../src/day07-matrix-blocking.cpp)，SHA-256 `9839e4d1de9d33e8fd22092850055949511ae06bc64520f28e6da95a6a2e4e18`；该文件是后续整合版，并非历史实测逐字源码。
- 构建（x64 Native Tools）：`cl /nologo /std:c++17 /EHsc /utf-8 /Od src\day07-matrix-blocking.cpp` 与相同参数下的 `/O2`；除优化选项外保持一致。
- 顺序：`01-o2`、`02-od`、`03-od`、`04-o2`；每次独立进程和输出目录，各 75 条正式记录；未删除样本。
- 每次程序先 pilot，再进行 64、128、256 阶的五种算法测量，每组 5 个批次，算法顺序轮换；每批次内重复多次并输出平均耗时。规模顺序固定。计时包括 C 清零、计算和批次循环开销；初始化与校验在计时外。每批仅校验最后一次结果。
- 两个优化设置下分别以相同编译选项构建 `tests/day07-correctness.cpp`，均输出 `PASS: 392 algorithm/input cases`；该独立测试覆盖整合版，不代表历史计时复现。
- 运行时未记录频率、温度、后台负载、电源状态或 CPU 亲和性；混合大小核和系统调度可能影响结果。只有每种构建两次进程运行，不能视作充分的统计证据。没有采集硬件计数器。

[分析报告](../../reports/2026-11-01-compiler-blocking-windows.md) · [01 /O2](01-o2.csv) · [02 /Od](02-od.csv) · [03 /Od](03-od.csv) · [04 /O2](04-o2.csv)
