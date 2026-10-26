# 元数据：Day 5 前一轮，只测 ikj

- 编号 / 日期：Day 5 之前单独交过的一轮，只测 `ikj`。CSV 和源码里都没有日历日期。验收包自己的日期是 2026-10-26，正文把这轮叫做「昨天」。目录名里的 2026-10-25 只跟着这个相对说法，不是 CSV 字段，也不是 ZIP 条目的修改时间（那些时间戳是打包时间）。
- 目标：在双版本对比之前，用只含 `ikj` 的程序做 64、128、256 的重复计时。
- 源码路径：[`code_history/day05/V01_yesterday_ikj_only.cpp`](../../../code_history/day05/V01_yesterday_ikj_only.cpp)。这是我此前提交的程序，按提交内容留着，里面不一定每一行都是我自己写的。它不在 `src/`，因为现在接着看的是今天的双版本。程序用 `ofstream` 写出的文件名是 `matmul_benchmark_raw.csv`。
- 编译器 / 参数：未记录。
- CPU / OS：未记录。验收包里有一张 Visual Studio Release | x64 截图，但没有写它属于这一轮。原图在 [`../2026-10-26-day05-loop-order-local/visual-studio-release-x64.png`](../2026-10-26-day05-loop-order-local/visual-studio-release-x64.png)，这里不复制。
- 矩阵生成：源码里是 `A[i][j] = i + j + 1`，`B[i][j] = 2 * i + j + 1`。
- 计时范围：源码把 C 清零和矩阵乘法放在 `matmul_ikj` 里，并注明这两部分都计入计时。
- 样本数 / 重复 / 单位：15 条。n = 64、128、256，各 5 次。重复次数是 64 阶 2208、128 阶 552、256 阶 84。`ms_per_multiply` 的单位是毫秒。
- 正确性检查：`C[0][0]` 和校验和。15 条状态都是 PASS。表头没有 `algorithm`，也没有逐元素比较列。
- 原始数据来源：[`day05-ikj-only-program-export.csv`](day05-ikj-only-program-export.csv) 是这轮程序直接导出的 CSV，按验收包原样放入。验收包里的文件名是 `04_实验数据/昨天_单算法测试/matmul_benchmark_raw.csv`。
- 统计方法：验收材料没有给这 15 条单独做中位数表。本目录不另算一套拿来和今天对照。15 条原值都在 CSV 里。
- 解释与限制：这不是 Day 4 的 ijk 基线，也不是今天双版本里的 `ikj`。Day 4 缺口说明里出现过的 `matmul_benchmark_raw.csv` 叫法，指的是还没上传的 Day 4 导出；文件名碰巧相同，内容不是这一份。日期、程序结构和检查项都和 2026-10-26 那 30 条不同，不排成同一轮算法对照。学习用时未记录。
