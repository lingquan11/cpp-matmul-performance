#!/usr/bin/env bash
# 编译 SIMD 对照脚手架，并跑尚未接入 smoke.sh 的检查。
# 不编译 src/ 以外的方式去改 smoke，不写 experiments/results，不产生性能数字。
# 核心未实现时不要把 tests/simd-factor-check.cpp 接进 tests/smoke.sh。待零醛实现后接入。
# 2026-11-22：行跨度 LDA 脚手架已归档到 archive/2026-11-scaffolds/，本脚本不再编译它（文件名保持不变）。

set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
mkdir -p "$root/build"
cxx=(g++ -std=c++17 -O2 -Wall -Wextra -pedantic)

"${cxx[@]}" -o "$root/build/simd-factor" "$root/templates/gpt/simd-factor.cpp"
"${cxx[@]}" -o "$root/build/simd-factor-novec" -DNO_VEC "$root/templates/gpt/simd-factor.cpp"
"${cxx[@]}" -o "$root/build/simd-factor-avx2" -mavx2 "$root/templates/gpt/simd-factor.cpp"
"${cxx[@]}" -o "$root/build/simd-factor-check" "$root/tests/simd-factor-check.cpp"
"${cxx[@]}" -DNO_VEC -o "$root/build/simd-factor-check-novec" "$root/tests/simd-factor-check.cpp"
"${cxx[@]}" -mavx2 -o "$root/build/simd-factor-check-avx2" "$root/tests/simd-factor-check.cpp"

echo "compiled simd-factor (V1/V0/V2 labels) and its check"

tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

set +e
"$root/build/simd-factor-check" >"$tmp/simd-check.txt"
simd_rc=$?
set -e
cat "$tmp/simd-check.txt"
if [[ "$simd_rc" -ne 0 ]]; then
  echo "simd-factor-check 退出码 $simd_rc" >&2
  exit 1
fi
grep -q '^SKIP:' "$tmp/simd-check.txt"
"$root/build/simd-factor-check-novec" | grep -q 'build 标记是 V0'
"$root/build/simd-factor-check-avx2" | grep -q 'build 标记是 V2'

out="$tmp/should-not-exist.csv"
set +e
"$root/build/simd-factor" --algo ikj --n 256 --runs 1 --out "$out" >"$tmp/simd-run.txt" 2>&1
simd_run=$?
set -e
cat "$tmp/simd-run.txt"
if [[ "$simd_run" -eq 0 || -e "$out" ]]; then
  echo "未实现的 simd-factor 不该成功，也不该写 CSV" >&2
  exit 1
fi
grep -q 'build=V1' "$tmp/simd-run.txt"

set +e
"$root/build/simd-factor-novec" --algo blocked_16 --n 1024 --out "$out" >"$tmp/simd-1024.txt" 2>&1
simd_1024=$?
set -e
cat "$tmp/simd-1024.txt"
if [[ "$simd_1024" -eq 0 || -e "$out" ]]; then
  echo "n=1024 未实现时不该写 CSV" >&2
  exit 1
fi
grep -q 'build=V0' "$tmp/simd-1024.txt"
grep -q 'n=1024' "$tmp/simd-1024.txt"

set +e
"$root/build/simd-factor" --n 64 --algo ikj --out "$out" >"$tmp/simd-bad.txt" 2>&1
simd_bad=$?
set -e
if [[ "$simd_bad" -ne 2 ]]; then
  echo "simd 非法 n 应退出 2，实际 $simd_bad" >&2
  exit 1
fi

python3 "$root/tests/test_scan_asm_keywords.py"

echo "simd 脚手架检查结束。SKIP 不是 PASS。没有写实验结果。"
