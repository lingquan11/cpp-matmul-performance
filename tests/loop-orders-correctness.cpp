// 六种循环顺序的正确性检查。不记时，不写 CSV。
//
// 它是什么：n ∈ {1,2,70,255,256} 上，把 kernel 的结果与独立 int64 参考比较。
// 谁写的：测试由 AI 维护。它不实现 jik / kij / jki / kji。
// 怎么编译和运行（仓库根目录）：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/loop-orders-correctness tests/loop-orders-correctness.cpp
//   build/loop-orders-correctness
// 限制：四个 TODO(零醛) kernel 未实现时打印 SKIP，退出码仍为 0。
// 已实现的 ijk、ikj 必须与 int64 参考一致，否则退出码 1。
// 本测试没有接进 tests/smoke.sh。待零醛实现后接入。
// 桩若在抛出「未实现」之前改写了 C，本测试失败，避免把假结果当成 SKIP。

#define EXP_LOOP_ORDERS_NO_MAIN
#include "../templates/gpt/exp-loop-orders.cpp"

#include <cstdint>
#include <iostream>

namespace {

constexpr int kSentinel = -123456789;
const int kSizes[] = {1, 2, 70, 255, 256};
const char* kPatterns[] = {"formula", "signed"};

std::int64_t expected[MAX_N][MAX_N];

void prepare(int n, int pattern) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (pattern == 0) {
                A[i][j] = i + j + 1;
                B[i][j] = 2 * i + j + 1;
            } else {
                A[i][j] = (i * 3 + j * 5) % 11 - 5;
                B[i][j] = (i * 7 + j * 2) % 13 - 6;
            }
        }
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::int64_t sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += static_cast<std::int64_t>(A[i][k]) * static_cast<std::int64_t>(B[k][j]);
            }
            expected[i][j] = sum;
        }
    }
}

bool region_is_sentinel(int n) {
    for (int i = 0; i < MAX_N; ++i) {
        for (int j = 0; j < MAX_N; ++j) {
            if (i < n && j < n) {
                continue;
            }
            if (C[i][j] != kSentinel) {
                return false;
            }
        }
    }
    return true;
}

bool matches(int n) {
    if (!region_is_sentinel(n)) {
        return false;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const std::int64_t cell = expected[i][j];
            if (cell < INT_MIN || cell > INT_MAX || C[i][j] != cell) {
                std::cerr << "FAIL C[" << i << "][" << j << "]=" << C[i][j]
                          << " expected=" << cell << "\n";
                return false;
            }
        }
    }
    return true;
}

void fill_sentinel() {
    for (int i = 0; i < MAX_N; ++i) {
        for (int j = 0; j < MAX_N; ++j) {
            C[i][j] = kSentinel;
        }
    }
}

}  // namespace

int main() {
    int passed = 0;
    int skipped = 0;
    const LoopOrder orders[] = {
        {"ijk", call_ijk},
        {"ikj", call_ikj},
        {"jik", matmul_jik},
        {"kij", matmul_kij},
        {"jki", matmul_jki},
        {"kji", matmul_kji},
    };

    for (int n : kSizes) {
        for (int pattern = 0; pattern < 2; ++pattern) {
            prepare(n, pattern);
            for (const LoopOrder& order : orders) {
                fill_sentinel();
                try {
                    order.fn(n);
                } catch (const NotImplementedError& ex) {
                    if (!region_is_sentinel(n)) {
                        std::cerr << "FAIL n=" << n << " pattern=" << kPatterns[pattern]
                                  << " algo=" << order.name
                                  << " 未实现桩改写了 C，不能 SKIP。\n";
                        return 1;
                    }
                    for (int i = 0; i < n; ++i) {
                        for (int j = 0; j < n; ++j) {
                            if (C[i][j] != kSentinel) {
                                std::cerr << "FAIL n=" << n << " algo=" << order.name
                                          << " 未实现桩写入了 C。\n";
                                return 1;
                            }
                        }
                    }
                    std::cout << "SKIP n=" << n << " pattern=" << kPatterns[pattern]
                              << " algo=" << order.name << " reason=" << ex.what() << "\n";
                    ++skipped;
                    continue;
                }
                if (!matches(n)) {
                    std::cerr << "FAIL n=" << n << " pattern=" << kPatterns[pattern]
                              << " algo=" << order.name << "\n";
                    return 1;
                }
                std::cout << "PASS n=" << n << " pattern=" << kPatterns[pattern]
                          << " algo=" << order.name << "\n";
                ++passed;
            }
        }
    }

    std::cout << "loop-orders correctness: PASS " << passed
              << " implemented cases; SKIP " << skipped
              << " unimplemented. No timings or CSV written.\n";
    return 0;
}
