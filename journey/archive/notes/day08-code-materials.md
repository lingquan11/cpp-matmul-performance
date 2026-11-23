# Day 8 代码材料说明

Day 8 学的是性能诊断和实验设计，没有写新的 C++ 程序。

- 今天没有提交新代码，也没有要求写新程序。
- 不建 `src/day08*`，不生成占位源码。
- 没有新的 GPT 代码模板。
- 归档包里带了一份 Day 5 循环顺序程序的历史参考快照 `matmul_benchmark_day5_loop_order.cpp`。它不是 Day 8 的新代码，Day 8 也没有改它，这次没有在 Visual Studio 里重新编译或运行。

## 那份 Day 5 快照怎么放

我把它和仓库里已有的 Day 5 程序比过：

| 文件 | SHA-256 | 行数 |
| --- | --- | ---: |
| [`src/day05-ijk-ikj-benchmark.cpp`](../src/day05-ijk-ikj-benchmark.cpp)（= [`code_history/day05/V02_today_ijk_ikj.cpp`](../code_history/day05/V02_today_ijk_ikj.cpp)） | `148f99bf…1030b` | 365 |
| 归档包快照 | `d2add857…3dabce` | 252 |

两份字节不同。差别在注释和排版：快照删了大部分中文注释，把多行语句并成一行。去掉注释和空白后逐个记号对比，只剩结尾一句输出：现行版写成 `"\nDay 5 实验完成。" << "原始数据已写入 "`，快照并成一个字符串 `"\nDay 5 实验完成。原始数据已写入 "`，打印出来的文字一样。计算、计时范围、CSV 列和文件名都没差别。

因为字节不同，两份都留着：现行版不动，快照原样放在 [`code_history/day05/matmul_benchmark_day5_loop_order-day08-snapshot.cpp`](../code_history/day05/matmul_benchmark_day5_loop_order-day08-snapshot.cpp)。正式 Day 5 CSV 仍然对应现行版。
