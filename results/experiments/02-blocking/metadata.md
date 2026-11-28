# 实验 02 元数据

- 日期：2026-10-27（学习记录中的 Day 7）
- 环境：数据包里没有记录 CPU、编译器版本和构建配置；[分块笔记](../../../journey/notes/blocking.md) 写的是与前几天同机（Windows，Intel Core Ultra 7 255HX，Visual Studio 2026 18.11.1，Release | x64）
- 算法与规模：`ijk`、`ikj`、`blocked_16` / `32` / `64` 同轮对比；n = 64、128、256；每组 5 个正式样本，共 75 条
- 计时范围：C 清零和乘法在计时内；输入初始化、正确性检查、校验和在计时外
- 正确性：75 条均为 status=PASS、elementwise_match=YES；另有 n = 70、b = 32 的边界测试，[截图](boundary-test-n70-b32.png)显示 PASS（不算性能样本）
- 原始数据：[`data/blocking.csv`](data/blocking.csv) 是程序直接导出的 CSV（程序写出名 `matmul_benchmark_day7_blocking.csv`，入库前名为 `day07-blocking-program-export.csv`），按原样复制，测量值没有改。[`data/summary.csv`](data/summary.csv) 和 [`speedup.svg`](speedup.svg) 由它生成，来源与口径记录在 [`data/figure-provenance.json`](data/figure-provenance.json)
- 源码：当时在聊天中提交的是修改过程中的中间版本（分块试测还在参考结果建立之前），原样保存在归档的 [`code_history/day07/V01_user_submitted_intermediate.cpp`](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/code_history/day07/V01_user_submitted_intermediate.cpp)；GPT 的分块片段在归档的 [`templates/gpt/day07-blocking-snippets.md`](https://github.com/lingquan11/cpp-matmul-performance/blob/archive-2026-11-23/journey/archive/templates/gpt/day07-blocking-snippets.md)。现在的 [`results/src/blocking.cpp`](../../src/blocking.cpp) 是事后的整合版，不是这组数据的逐字源码
