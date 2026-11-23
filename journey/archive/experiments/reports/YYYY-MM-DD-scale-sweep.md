# 规模扫描报告模板（AI 维护，无实测）

日期、环境、参数、结果：待测。结论：待零醛实测后填写。

- CP1：使用 Day 7 原程序演练绑核，与 11-01 /O2 中位数比较，允许偏差 ±15%；未通过则停。
- CP2：新程序在 n=256 与历史数据对照；先确认输入和行跨度不同的可比性。
- CP3：完成正确性验证及测量地基后，进行三轮探索；正式 R 判定需十个进程，三轮不足以作 R 判定。
- CP4：记录环境与结果，填写报告；找不到交叉点也是结果。

[规则 R 草案](https://github.com/lingquan11/cpp-matmul-performance/blob/ideas/matmul-1002-i1-measurement-baseline/experiments/reports/measurement-rules.md)：10/10 同侧且中位比偏离至少 5%，否则不判；待零醛确认。

脚手架不含实现或有效样本。是否扩至 2048、输入上界与正式阈值由零醛决定。
