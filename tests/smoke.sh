#!/usr/bin/env bash
# Compile every current program in src/. Run only Day 1 and Day 2,
# which have fixed output. Do not run Day 4 or Day 5: those write
# timings and would mix new samples into the archived results.
set -euo pipefail

root="$(cd "$(dirname "$0")/.." && pwd)"
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

shopt -s nullglob
files=("$root"/src/*.cpp)
if ((${#files[@]} == 0)); then
  echo "src/ has no .cpp files" >&2
  exit 1
fi

for src in "${files[@]}"; do
  base="$(basename "$src" .cpp)"
  g++ -std=c++17 -O2 -o "$tmp/$base" "$src"
done

day01="$("$tmp/day01-matrix")"
if [[ "$day01" != "11" ]]; then
  printf 'Day 1: expected 11, got %q\n' "$day01" >&2
  exit 1
fi

day02="$("$tmp/day02-matrix")"
expected="$(printf '11 10 \n19 30 ')"
if [[ "$day02" != "$expected" ]]; then
  printf 'Day 2 mismatch\nexpected %q\ngot      %q\n' "$expected" "$day02" >&2
  exit 1
fi

echo "smoke ok: compiled ${#files[@]} files under src/; Day 1 printed 11; Day 2 printed the 2x2 result"
echo "Day 4 and Day 5 were compiled only. Timing reproduction: not verified."
