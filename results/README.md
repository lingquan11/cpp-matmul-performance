# 学习成果

| # | 做了什么 | 结论 | 目录 |
|---|---|---|---|
| 01 | 循环顺序 ijk vs ikj | `ikj` 快 6.85–9.38 倍（n = 64–256，中位数） | [experiments/01-loop-order](experiments/01-loop-order/) |
| 02 | 分块 b = 16 / 32 / 64 | n = 128、256 时分块比 `ikj` 慢 11–60%；n = 64、b = 64 基本持平 | [experiments/02-blocking](experiments/02-blocking/) |
| 03 | 编译选项 /O2 vs /Od | `/O2` 下分块仍慢于 `ikj`；`/Od` 下 n = 256 的 `ikj` 从约 1.3 ms 变为 22.8–24.8 ms | [experiments/03-compiler-flags](experiments/03-compiler-flags/) |

进行中：04 规模扫描（n 到 2048）、05 SIMD，见 [start/STATE.md](../start/STATE.md)。

## 复现
```bash
bash results/tests/smoke.sh
```
编译全部程序并运行分块正确性测试（不运行计时）。中位数可以用 `python3 results/scripts/summarize-medians.py` 从 CSV 重算。

## 目录
- `src/`：矩阵乘法程序
- `harness/`：AI 写的计时框架（`[ai]`），供 04、05 使用
- `tests/`：正确性测试
- `experiments/`：一个实验一个目录（README + data/ + 图）
- `scripts/`：汇总、作图和 Windows 运行脚本
