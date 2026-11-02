# 测量地基元数据（模板）

这是什么：一组实测目录里的环境记录模板。目录名里的 `YYYY-MM-DD` 等我真正开跑那天再改。
谁写的：模板由 AI 起草。下面的值都还不是测量。
怎么填：在我的 Windows 电脑上跑完 [`scripts/run-core-scan.bat`](../../../scripts/run-core-scan.bat) 和 [`scripts/run-c0-c1.bat`](../../../scripts/run-c0-c1.bat) 之后，把对应字段改成当时的记录。判定口径见 [`measurement-rules.md`](../../reports/measurement-rules.md)。
限制：没有实测之前保持「待测」。不要把云端、Linux、虚拟机或格式夹具里的数字写进来。不倒填电源、频率或绑核。本目录现在没有 CSV。

- 日期：待测
- exe SHA256：待测
- 编译命令：待测
- 编译器版本：待测
- 电源方案：待测
- 是否插电：待测
- 频率控制：待测
- 绑核掩码：待测
- P/E 簇判断依据：待测

`core-scan/`、`c0/`、`c1/` 里只有占位。正式 CSV 等我在本机跑完再放进来，一个进程一个子目录。
