#!/usr/bin/env bash
# 编译输出驻留脉动阵列脚手架。
# 不编译 src/，不调用 tests/smoke.sh，不写 experiments/results，不产生性能数字。
# 核心未实现时不要把 tests/systolic-model-check.cpp 接进 smoke.sh。待零醛实现后接入。

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$root/build"

g++ -std=c++17 -O2 -Wall -Wextra -pedantic \
    -o "$root/build/systolic-os-model" \
    "$root/templates/gpt/systolic-os-model.cpp"

g++ -std=c++17 -O2 -Wall -Wextra -pedantic \
    -o "$root/build/systolic-model-check" \
    "$root/tests/systolic-model-check.cpp"

echo "compiled: build/systolic-os-model"
echo "compiled: build/systolic-model-check"
echo "核心未实现时："
echo "  ./build/systolic-model-check            # 应打印 SKIP，退出码 0，不是 PASS"
echo "  ./build/systolic-os-model --n 2 --P 2   # 应打印未实现，退出码非 0，不写 CSV"
echo "不要把上述运行耗时写进实验结果。"
