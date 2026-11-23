# Day 8 材料完整性检查

对照的是 `PROJECT_STATE.md` 和 Day 5–Day 7 的归档方式。没有的项保持缺失，不写成「完整」。

状态用：已具备 / 本日不适用 / 可从仓库历史补齐 / 需要用户补交 / 暂时无法核实。

| 检查项 | 状态 | 证据或原因 |
| --- | --- | --- |
| 学习时间 | 已具备 | 5 小时（300 分钟），归档包 README、[`day08-record.md`](../day08-record.md)、[`day08-notes.md`](../day08-notes.md) 一致 |
| 学习日期 | 需要用户补交 | 归档包没写 Day 8 是哪一天。不拿 ZIP 条目时间或上传时间代替，记「未记录」 |
| 对话记录 | 已具备（有缺口） | [`day08-dialogue-record.md`](../day08-dialogue-record.md)。只含看得见的部分，不是逐字无缺口导出 |
| 原始聊天导出（无缺口逐字） | 需要用户补交 | 归档包没有原始导出；聊天里有被系统折叠成 “Skipped N messages” 的一段，正文看不到。仓库里此前也没有 Day 8 原始导出 |
| 问题回答（我的原话） | 已具备（有缺口） | [`day08-visible-user-turns.md`](../day08-visible-user-turns.md)，37 条；第 32、34、35、36 条含概述，已注明 |
| 笔记 | 已具备 | [`day08-notes.md`](../day08-notes.md) |
| 验收记录 | 已具备 | [`day08-record.md`](../day08-record.md)：综合考核通过 |
| 学习报告 | 已具备 | [`experiments/reports/day08-performance-analysis.md`](../../experiments/reports/day08-performance-analysis.md)。方法报告，不是实测报告 |
| 环境信息 | 可从仓库历史补齐（已补） | 用户确认与此前各日不变。CPU 型号沿用 [`experiment-environment.md`](experiment-environment.md) 里的 Intel Core Ultra 7 255HX；Visual Studio 2026 18.11.1。Day 8 没有重新测或重新确认型号 |
| MSVC toolset 独立版本号 | 需要用户补交 | 和此前一样未记录 |
| 最终代码或无代码修改说明 | 已具备 | 无新代码，说明在 [`day08-code-materials.md`](../day08-code-materials.md) |
| Day 5 历史快照与仓库源码比较 | 已具备 | 字节不同、计算逻辑相同，两份都留，见 [`day08-code-materials.md`](../day08-code-materials.md) |
| 原始 CSV | 本日不适用 | Day 8 没有正式硬件测量。对话里的 IPC、MLP、带宽、Pipeline Slots、A/B/C/D 等数字是教学示例，不进 `experiments/results/` |
| perf / 硬件计数器原始输出 | 本日不适用 | 今天只学方法，没有实际采集 |
| 截图 | 本日不适用 | 没有实验运行，归档包里也没有截图 |
| GitHub 文件索引 | 已具备 | [`daily-task-records.md`](../daily-task-records.md)、[`notes/README.md`](../README.md)、[`experiments/reports/README.md`](../../experiments/reports/README.md)、[`PROJECT_STATE.md`](../../PROJECT_STATE.md) 已加 Day 8 |
| 归档包本身 | 已具备 | 13 个文件，`FILE_CHECKSUMS.md` 里 12 个文件的 SHA-256 全部对上。ZIP、清单和校验表不入库，照此前做法 |
| 掌握情况是否等于独立完成真实 perf 采集 | 暂时无法核实 | 验收来自辅导问答；没有真实 perf 采集，不写成已经独立掌握 |

## 归档包文件去向

| 归档包路径 | 仓库位置 | 处理 |
| --- | --- | --- |
| `notes/day08_learning_notes.md` | [`notes/day08-notes.md`](../day08-notes.md) | 新增，改一句第三人称 |
| `notes/day08_qa_and_assessment.md` | [`notes/day08-record.md`](../day08-record.md) | 新增，补「基本信息」 |
| `notes/day08_dialogue_record.md` | [`notes/day08-dialogue-record.md`](../day08-dialogue-record.md) | 新增，改成第一人称，事实不动 |
| `notes/day08_visible_user_turns.md` | [`notes/day08-visible-user-turns.md`](../day08-visible-user-turns.md) | 新增，原话不动，概述条目加注 |
| `reports/day08_performance_analysis.md` | [`experiments/reports/day08-performance-analysis.md`](../../experiments/reports/day08-performance-analysis.md) | 新增，加来源说明。日期未记录，文件名不带日期 |
| `environment/day08_environment.md` | 并入 [`experiment-environment.md`](experiment-environment.md) | 追加 Day 8 一节 |
| `results/RESULTS_STATUS.md` | 并入 [`experiments/results/README.md`](../../experiments/results/README.md) | 追加一句；不建结果目录 |
| `code_history/day05/matmul_benchmark_day5_loop_order.cpp` | [`code_history/day05/matmul_benchmark_day5_loop_order-day08-snapshot.cpp`](../../code_history/day05/matmul_benchmark_day5_loop_order-day08-snapshot.cpp) | 新增，字节原样 |
| `code_history/README.md` | 并入 [`code_history/day05/README.md`](../../code_history/day05/README.md) | 追加一行 |
| `handoff/github_maintenance_agent_prompt.md` | [`notes/handoff-day08-github-maintenance-agent-prompt.md`](../handoff-day08-github-maintenance-agent-prompt.md) | 新增，字节原样；GPT 写的维护提示词 |
| `README.md`、`ARCHIVE_MANIFEST.md`、`FILE_CHECKSUMS.md` | 不入库 | 只用于核对 |
