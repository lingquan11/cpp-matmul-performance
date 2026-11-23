#!/usr/bin/env bash
# 编译 results/src/ 下全部程序，并运行分块正确性测试。不运行任何计时程序。
# 用法（仓库根目录）：bash results/tests/smoke.sh
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

shopt -s nullglob
files=("$root"/src/*.cpp)
if ((${#files[@]} == 0)); then
  echo "results/src/ has no .cpp files" >&2
  exit 1
fi
for src in "${files[@]}"; do
  g++ -std=c++17 -O2 -o "$tmp/$(basename "$src" .cpp)" "$src"
done

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o "$tmp/blocking-correctness" "$root/tests/blocking-correctness.cpp"
"$tmp/blocking-correctness"

# 非法参数必须在访问固定容量数组之前被拒绝（退出码 2）。
for args in "0 32" "257 32" "70 0" "70 -1" "70 258" "70 1x" "1" "1 2 3"; do
  read -r -a values <<< "$args"
  set +e
  "$tmp/blocking-correctness" "${values[@]}" >/dev/null 2>&1
  status=$?
  set -e
  if ((status != 2)); then
    echo "Expected usage exit 2, got $status: $args" >&2
    exit 1
  fi
done

echo "smoke ok: compiled ${#files[@]} programs; blocking correctness passed (timing not run)"
