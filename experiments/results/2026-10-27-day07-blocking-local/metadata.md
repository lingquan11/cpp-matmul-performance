# Day 7 分块实验元数据

- 日期：2026-10-27
- 主题：分块矩阵乘法（blocked_16 / blocked_32 / blocked_64）与 ijk、ikj 同轮对比
- 环境：未记录（本包未写 CPU / 编译器版本；若与 Day 4/5 同机，待用户确认后再补）
- 配置：未记录（Release | x64 等截图未随本包提供）
- 计时口径：含结果矩阵 C 清零与矩阵乘法；输入初始化、正确性检查、校验和在计时外
- 矩阵规模：64、128、256
- 算法：ijk、ikj、blocked_16、blocked_32、blocked_64
- 每组合正式样本：5 次；合计 75 条
- 正确性：75 条 status=PASS，elementwise_match=YES
- 边界测试：n=70、block_size=32，截图显示 PASS（非正式性能样本）

## 文件

| 文件 | 来源类型 | 原程序写出名 |
| --- | --- | --- |
| `day07-blocking-program-export.csv` | 程序直接导出 | `matmul_benchmark_day7_blocking.csv` |
| `day07-boundary-test-n70-block32.png` | 控制台边界测试截图 | — |

字节内容按归档包原样复制，未改实测值。

## 源码说明

用户在聊天中提交的是修改过程中的中间版本（分块试测曾位于 Reference / reference_checksum 建立之前），按原样放在 `code_history/day07/V01_user_submitted_intermediate.cpp`。未收到修正后的完整整合源码，故不写入 `src/` 冒充现行成品。GPT 分块片段在 `templates/gpt/day07-blocking-snippets.md`。
