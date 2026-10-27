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

用户没有在聊天中再次提供修正后的完整整合源码，因此不将中间代码改写或冒充为最终用户产出。没有新增硬件型号、编译器版本或缓存性能计数器数据，故不作相关实测结论。
