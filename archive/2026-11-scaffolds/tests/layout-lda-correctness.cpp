// 行跨度脚手架的正确性检查。测试适配由 AI 维护，不代替零醛实现 kernel。
//
// 这是什么：核心未实现时输出 SKIP。实现后用 int64 参考逐元素比较 n=70，以及 lda>n。
// 谁写的：AI。标 TODO(零醛) 的乘法不是这份测试代写的。
//
// 编译（仓库根目录）：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/layout-lda-correctness tests/layout-lda-correctness.cpp
// 或：bash scripts/build-lda-simd.sh
//
// 限制：不要把本文件接进 tests/smoke.sh，直到两个 kernel 实现且 kLayoutLdaImplemented 改为 true。
// 待零醛实现后接入。SKIP 不是 PASS。本测试不写 CSV，也不计时。
// 初始化沿用 day07：A=i+j+1，B=2*i+j+1。n<=256 的保守上界沿用 tests/README.md 已写下的
// 256*511*766=100205056，这里只再算一遍这个乘法，不是新的测量。

#define main layout_lda_program_main
#include "../templates/gpt/layout-lda.cpp"
#undef main

#include <iostream>
#include <string>
#include <vector>

namespace {

bool fail(const std::string& message) {
    std::cerr << "FAIL: " << message << '\n';
    return false;
}

bool check_formula() {
    const long long bound = 256LL * 511 * 766;
    if (bound != 100205056LL) {
        return fail("day07 上界 256*511*766 不是 README 里的 100205056");
    }
    if (bound > 2147483647LL) {
        return fail("n=256 的保守上界超出了 32 位有符号整数");
    }

    int lda = 0;
    std::string error;
    if (!layout_resolve_lda("n", 64, lda, error) || lda != 64) {
        return fail("lda 记号 n 在 n=64 时应是 64");
    }
    if (!layout_resolve_lda("n+16", 64, lda, error) || lda != 80) {
        return fail("lda 记号 n+16 在 n=64 时应是 80");
    }
    if (!layout_resolve_lda("256", 64, lda, error) || lda != 256) {
        return fail("lda 记号 256 在 n=64 时应是 256");
    }
    if (!layout_resolve_lda("n+16", 256, lda, error) || lda != 272) {
        return fail("lda 记号 n+16 在 n=256 时应是 272");
    }
    if (!layout_resolve_lda("n", 256, lda, error) || lda != 256) {
        return fail("n=256 时记号 n 应和 256 相同");
    }
    if (layout_resolve_lda("n+15", 64, lda, error)) {
        return fail("n+15 不该被当成合法记号");
    }
    if (layout_resolve_lda("32", 64, lda, error)) {
        return fail("lda 小于 n 时不该通过");
    }

    std::vector<int> A(2 * 6);
    std::vector<int> B(2 * 6);
    layout_fill_inputs(A.data(), B.data(), 2, 6);
    const int expect_a[2][2] = {{1, 2}, {2, 3}};
    const int expect_b[2][2] = {{1, 2}, {3, 4}};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (A[static_cast<size_t>(layout_index(i, 6, j))] != expect_a[i][j] ||
                B[static_cast<size_t>(layout_index(i, 6, j))] != expect_b[i][j]) {
                return fail("n=2 的初始化不是 day07 公式");
            }
        }
        if (A[static_cast<size_t>(layout_index(i, 6, 2))] != 0) {
            return fail("lda 多出来的列不该填进公式");
        }
    }

    A[static_cast<size_t>(layout_index(0, 6, 2))] = 999999;
    B[static_cast<size_t>(layout_index(1, 6, 2))] = 999999;
    std::vector<long long> wide;
    layout_reference_i64(A.data(), B.data(), 2, 6, wide);
    const long long expect_c[4] = {7, 10, 11, 16};
    for (int index = 0; index < 4; ++index) {
        if (wide[static_cast<size_t>(index)] != expect_c[index]) {
            return fail("n=2 的 int64 参考不是手算的 7 10 / 11 16");
        }
    }

    std::vector<int> A_tight(4);
    std::vector<int> B_tight(4);
    layout_fill_inputs(A_tight.data(), B_tight.data(), 2, 2);
    std::vector<long long> tight;
    layout_reference_i64(A_tight.data(), B_tight.data(), 2, 2, tight);
    if (tight != wide) {
        return fail("填充列被参考读进去了");
    }

    const std::string header = kLayoutCsvHeader;
    if (header.find("lda") == std::string::npos || header.find("ms_per_multiply") == std::string::npos) {
        return fail("CSV 表头缺列");
    }
    return true;
}

bool compare_case(int n, int lda) {
    std::vector<int> A(static_cast<size_t>(n * lda));
    std::vector<int> B(static_cast<size_t>(n * lda));
    std::vector<int> C(static_cast<size_t>(n * lda), 7);
    layout_fill_inputs(A.data(), B.data(), n, lda);
    std::vector<long long> reference;
    layout_reference_i64(A.data(), B.data(), n, lda, reference);
    matmul_ijk_lda(A.data(), B.data(), C.data(), n, lda);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (C[static_cast<size_t>(layout_index(i, lda, j))] !=
                reference[static_cast<size_t>(i * n + j)]) {
                return fail("ijk 与 int64 参考不一致");
            }
        }
    }
    std::fill(C.begin(), C.end(), 7);
    matmul_ikj_lda(A.data(), B.data(), C.data(), n, lda);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (C[static_cast<size_t>(layout_index(i, lda, j))] !=
                reference[static_cast<size_t>(i * n + j)]) {
                return fail("ikj 与 int64 参考不一致");
            }
        }
    }
    return true;
}

}  // namespace

int main() {
    using Ijk = void (*)(int*, int*, int*, int, int);
    Ijk ijk = &matmul_ijk_lda;
    Ijk ikj = &matmul_ikj_lda;
    if (ijk == nullptr || ikj == nullptr) {
        std::cerr << "FAIL: kernel 符号不存在\n";
        return 1;
    }
    if (!check_formula()) {
        return 1;
    }
    if (!kLayoutLdaImplemented) {
        std::cout
            << "SKIP: kernel 未实现（TODO(零醛)：matmul_ijk_lda、matmul_ikj_lda）。这不是 PASS。\n"
            << "已核对 day07 初始化公式，以及 n=2、lda=6 时填充列不进 int64 参考。"
               "n=70 和 lda>n 的逐元素比对还没执行。\n";
        return 0;
    }
    const int n = 70;
    const int ldas[] = {70, 70 + 16, 256};
    for (int lda : ldas) {
        if (!compare_case(n, lda)) {
            std::cerr << "FAIL: n=70 lda=" << lda << "\n";
            return 1;
        }
    }
    std::cout << "PASS: n=70 且 lda 为 70、86、256 时，ijk 与 ikj 都和 int64 参考一致。\n";
    return 0;
}
