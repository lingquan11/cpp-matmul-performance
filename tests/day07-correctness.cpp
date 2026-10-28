// Maintenance test adapter, not a new student algorithm implementation.
// Include the existing implementation without invoking its benchmark main.
// This avoids changing the archived learning program just to expose test hooks.
#define main day07_benchmark_main
#include "../src/day07-matrix-blocking.cpp"
#undef main

#include <climits>
#include <cstdint>
#include <string>

namespace {
constexpr int sentinel = -123456789;
std::int64_t oracle[MAX_N][MAX_N] = {};
const char* patterns[] = {"formula", "zero", "identity", "signed"};

void prepare(int n, int pattern) {
    for (int i = 0; i < MAX_N; ++i) {
        for (int j = 0; j < MAX_N; ++j) {
            A[i][j] = B[i][j] = sentinel;
            if (i < n && j < n) {
                A[i][j] = pattern == 3 ? (i * 3 + j * 5) % 11 - 5 : i + j + 1;
                B[i][j] = pattern == 1 ? 0 :
                          pattern == 2 ? (i == j ? 1 : 0) :
                          pattern == 3 ? (i * 7 + j * 2) % 13 - 6 : 2 * i + j + 1;
            }
        }
    }
    // Independent wide dot products: neither the production ijk result nor
    // its checksum is used as the oracle. Products are widened BEFORE multiply.
    for (int j = 0; j < n; ++j) {
        for (int i = 0; i < n; ++i) {
            std::int64_t sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += static_cast<std::int64_t>(A[i][k]) * B[k][j];
            }
            oracle[i][j] = sum;
        }
    }
}

bool check(int n, int block, int pattern, int algorithm) {
    // A nonzero fill catches forgotten clearing and out-of-active-region writes.
    for (int i = 0; i < MAX_N; ++i) {
        for (int j = 0; j < MAX_N; ++j) C[i][j] = sentinel;
    }
    if (algorithm == 0) matmul_ijk(n);
    else if (algorithm == 1) matmul_ikj(n);
    else matmul_blocked(n, block);

    for (int i = 0; i < MAX_N; ++i) {
        for (int j = 0; j < MAX_N; ++j) {
            const std::int64_t expected = i < n && j < n ? oracle[i][j] : sentinel;
            if (expected < INT_MIN || expected > INT_MAX || C[i][j] != expected) {
                std::cerr << "FAIL n=" << n << " block=" << block
                          << " input=" << patterns[pattern] << " algorithm=" << algorithm
                          << " C[" << i << "][" << j << "]=" << C[i][j]
                          << " expected=" << expected << '\n';
                return false;
            }
        }
    }
    return true;
}

bool parse_positive(const char* text, int limit, int& value) {
    const std::string arg(text);
    if (arg.empty()) return false;
    value = 0;
    for (char ch : arg) {
        if (ch < '0' || ch > '9') return false;
        const int digit = ch - '0';
        if (value > (limit - digit) / 10) return false;
        value = value * 10 + digit;
        if (value > limit) return false;
    }
    return value > 0;
}

bool run_size(int n, int requested_block, int& count) {
    const int blocks[] = {1, 16, 32, 64, n + 1};
    for (int pattern = 0; pattern < 4; ++pattern) {
        prepare(n, pattern);
        for (int algorithm = 0; algorithm < 2; ++algorithm) {
            if (!check(n, 0, pattern, algorithm)) return false;
            ++count;
        }
        if (requested_block) {
            if (!check(n, requested_block, pattern, 2)) return false;
            ++count;
        } else {
            for (int block : blocks) {
                if (!check(n, block, pattern, 2)) return false;
                ++count;
            }
        }
    }
    return true;
}
} // namespace

int main(int argc, char* argv[]) {
    int n = 0, block = 0, count = 0;
    if (argc != 1 && (argc != 3 || !parse_positive(argv[1], MAX_N, n) ||
                      !parse_positive(argv[2], MAX_N + 1, block))) {
        std::cerr << "Usage: day07-correctness [n block_size]\n"
                  << "n: 1..256; block_size: 1..257; decimal integers only.\n";
        return 2;
    }
    if (argc == 3) {
        if (!run_size(n, block, count)) return 1;
        std::cout << "n=" << n << " block_size=" << block << '\n';
    } else {
        const int sizes[] = {1, 2, 15, 16, 17, 31, 32, 33, 63, 64, 65, 70, 255, 256};
        for (int size : sizes) {
            if (!run_size(size, 0, count)) return 1;
        }
    }
    std::cout << "PASS: " << count << " algorithm/input cases; exact int64 oracle; "
              << "active-region and output-sentinel checks. No timings or CSV written.\n";
    return 0;
}
