# 行跨度 LDA 元数据（模板）

这是什么：一组实测目录里的环境记录模板。目录名里的 `YYYY-MM-DD` 等我真正开跑那天再改。
谁写的：模板由 AI 起草。下面的值都还不是测量。
怎么填：在我的 Windows 电脑上跑完 [`scripts/run-layout-lda.bat`](../../../scripts/run-layout-lda.bat) 之后，把对应字段改成当时的记录。
限制：没有实测之前保持「待测」。不要把云端、Linux、虚拟机或格式夹具里的数字写进来。不倒填电源、频率或绑核。本目录现在没有 CSV。

- 日期：待测
- exe SHA256：待测
- 编译命令：待测
- 编译器版本：待测
- 电源方案：待测
- 是否插电：待测
- 频率控制：待测
- 绑核掩码（C1）：待测
- 轮间冷却秒数：待测

判定口径：10 个进程的中位数。中位数算法依赖尚未合并的分支 `ideas/matmul-1002-i1-measurement-baseline` 里的 `experiments/reports/measurement-rules.md`（规则 R）。本目录不复制那份草案。行跨度自己的阈值是 0.90 和 0.97–1.03，不是规则 R 的 5%。

编译器闸门不在这里再做一遍。和 SIMD 对照共用 [`scripts/vec-report.bat`](../../../scripts/vec-report.bat)。

硬件事实指向 [`notes/dayNN-hw-facts.md`](../../../notes/dayNN-hw-facts.md)。L1D 容量和相联度仍是待测。

组冲突模型和 4 KB 别名并列写在报告模板里，都不算已经选中的结论。

计时实验：无新进展。
