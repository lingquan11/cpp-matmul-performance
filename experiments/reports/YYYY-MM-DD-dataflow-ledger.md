# 数据流账本报告（模板）

它是什么：等我实测之后再另存成带真实日期的报告。现在这个文件名里的 `YYYY-MM-DD` 就是占位，不是某一次实验的日期。
谁写的：空位留给我。模板由 AI 放在这里，不是已完成的分析。
怎么用：测量当天复制到 `experiments/reports/YYYY-MM-DD-dataflow-ledger.md` 这种真实日期的文件名，原始 CSV 另建 `experiments/results/YYYY-MM-DD-<experiment>-<environment>/`，里面写 `metadata.md`。不要覆盖本模板，也不要覆盖旧报告。
限制：本页没有实测数字。结论待零醛实测后填写。本模板不含 SCALE-Sim，也不把 SCALE-Sim 排进这一次的工作；12 月 10 日之前做不做，按仓库里已经写过的计划，由我决定。

写循环顺序和数据流时用「类比于」，不用「就是」。

## 我测了什么

待零醛实测后填写。

- 命令：
- 环境：
- 日期：
- 结果目录：

## 计数口径（待零醛确认）

- C 的部分和，第一趟是否计读？
- 只计片外/缓冲访问，还是也计 PE 间传递？

未确认前，`count_os`、`count_ws`、`count_is` 保持未实现。纸笔表在 [`notes/dayNN-dataflow-derivation.md`](../../notes/dayNN-dataflow-derivation.md)。别人给的速度排名和账本总数只放在那份笔记的对照小节，不写进下面的结论。

## CPU 隐式复用 vs NPU 显式调度

三条都空着。

1. 差异一：
2. 差异二：
3. 差异三：

## 六种循环顺序

类比于 OS、WS、IS 的对应，等我把纸笔表填完再写。这里不预填。

## 结论

待零醛实测后填写。
