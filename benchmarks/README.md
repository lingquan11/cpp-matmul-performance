# 计时驱动

这里以后放我自己在本地测稳、决定公开的正式计时驱动。现在只有这份说明，没有可跑脚本。

正式可跑驱动等我本地测稳后再放进来。仓库里没有云端可跑的基准。以前从 main 拿掉的计时脚手架（`benchmarks/run_bench.py` 和云端计时 workflow）不恢复。

[`.github/workflows/compile.yml`](../.github/workflows/compile.yml) 只做编译检查，调用 [`tests/smoke.sh`](../tests/smoke.sh)。它不是性能测试，也不是计时基准。Day 4、Day 5 的计时复现：未验证。
