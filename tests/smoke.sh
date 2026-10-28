#!/usr/bin/env bash
# Compile every current program in src/. Run Day 1, Day 2 and the
# separate Day 7 correctness adapter. Never invoke a benchmark main.
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

g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o "$tmp/day07-correctness" "$root/tests/day07-correctness.cpp"
"$tmp/day07-correctness"

# Reject bad input before touching the fixed-capacity arrays or block loops.
for args in "0 32" "257 32" "70 0" "70 -1" "70 258" "70 1x" "1" "1 2 3"; do
  read -r -a values <<< "$args"
  if "$tmp/day07-correctness" "${values[@]}"; then
    echo "Expected invalid arguments to fail: $args" >&2
    exit 1
  else
    status=$?
    if ((status != 2)); then
      echo "Expected usage exit 2, got $status: $args" >&2
      exit 1
    fi
  fi
done

echo "smoke ok: compiled ${#files[@]} src files; Day 1/2 output and Day 7 correctness passed"
echo "Day 4/5/7 benchmark main functions were not run. Timing reproduction: not verified."
