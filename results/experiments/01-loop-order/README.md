# 01 循环顺序：ijk vs ikj

**问题**：只交换循环顺序（不改算法），按行存储的矩阵乘法能快多少？

**做法**：`int` 方阵，n = 64 / 128 / 256；同一次运行中交替测 `ijk` 和 `ikj`，每种 5 个样本，取 `ms_per_multiply` 的中位数。计时包含 C 清零和乘法。30 条全部通过逐元素校验。

## 结果
| n | ijk 中位数 (ms) | ikj 中位数 (ms) | ijk / ikj |
|---:|---:|---:|---:|
| 64 | 0.140724 | 0.020545 | 6.85× |
| 128 | 1.375541 | 0.162732 | 8.45× |
| 256 | 11.333944 | 1.207955 | 9.38× |

**结论**：`ikj` 的内层循环沿 B 和 C 的行连续访问，比 `ijk` 快 6.85–9.38 倍，规模越大差距越大。

## 边界
- 单次运行，CPU 频率、负载、绑核没有控制
- 只测了耗时，不能直接证明缓存未命中次数的变化
- 源码是辅导过程中提交的版本，不是每一行都由我独立完成

## 数据与复现
- 原始数据：[data/loop-order.csv](data/loop-order.csv)，环境：[metadata.md](metadata.md)（Windows，Core Ultra 7 255HX，VS 2026 Release x64）
- 程序：[results/src/loop-order.cpp](../../src/loop-order.cpp)；重算中位数：`python3 results/scripts/summarize-medians.py`
