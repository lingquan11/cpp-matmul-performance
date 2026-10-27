# 代码与材料说明

## 用户提交代码

`02_用户提交代码/day07_user_submitted_intermediate.cpp` 是本次对话中提供的源码中间版本，按原样保存。该中间版本曾把分块试测放在 `Reference` 与 `reference_checksum` 建立之前，因此不应当作为最终整合源码直接使用。

## GPT 模板

`03_GPT模板/day07_gpt_template_snippets.md` 单独保存 GPT 提供的分块函数、边界测试、试测顺序和正式测量调度片段。它们是指导模板，不代表用户独立编写的代码，也不是一个完整的单文件可编译程序。

## 实验数据

- `04_实验数据/matmul_benchmark_day7_blocking.csv`：本次正式原始数据，原样复制。
- `04_实验数据/day07_performance_analysis.md`：根据原始 CSV 计算的中位数和相对耗时分析。
- `05_测试证据/day07_boundary_test_n70_block32.png`：控制台边界测试通过截图。

## 未记录事项

用户没有在聊天中再次提供修正后的完整整合源码，因此不将中间代码改写或冒充为最终用户产出。没有新增缓存性能计数器数据，故不作 cache miss 的实测结论。

## 2026-10-27 补缴的 GPT 整合版

[`src/day07-matrix-blocking.cpp`](../src/day07-matrix-blocking.cpp) 是根据学习对话整理的 GPT 整合版，不是上面的用户中间版，也不是用户本机逐字源码。仓库 `src/` 用连字符文件名。

归档生成环境曾用 `g++ -std=c++17 -O2 -Wall -Wextra -pedantic` 编译并运行，边界测试通过，75 条正式样本全部 PASS。没有在用户本机 Visual Studio 重新构建。正式 CSV 仍是 [`experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv`](../experiments/results/2026-10-27-day07-blocking-local/day07-blocking-program-export.csv)，不用新跑结果覆盖。

环境沿用此前已提交记录，并确认 Day 4–Day 7 同机同硬件：Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release / x64。MSVC toolset 独立版本号未记录。不把 IDE 版本等同于 toolset。说明在 [`supplements/experiment-environment.md`](supplements/experiment-environment.md)。
