// SIMD 对照脚手架的检查。测试适配由 AI 维护，不代替零醛实现 kernel。
//
// 这是什么：核心未实现时输出 SKIP。确认 build 标记、n=256 可以沿用 day07 公式、n=1024 不行。
// 谁写的：AI。
//
// 编译：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/simd-factor-check tests/simd-factor-check.cpp
// 或：bash scripts/build-lda-simd.sh
//
// 限制：不要接进 tests/smoke.sh，直到 kernel 实现且 kSimdKernelImplemented 改为 true。
// 待零醛实现后接入。SKIP 不是 PASS。本测试不计时，也不写 CSV。
// 这里的整数上界是溢出检查，不是耗时。

#define main simd_factor_program_main
#include "../templates/gpt/simd-factor.cpp"
#undef main

#include <iostream>
#include <string>

namespace {

bool fail(const std::string& message) {
    std::cerr << "FAIL: " << message << '\n';
    return false;
}

bool check_bounds() {
    if (!simd_day07_formula_fits_int(256) || !simd_day07_formula_fits_int(1)) {
        return fail("n=256 应该还能用 day07 的 int 公式");
    }
    if (simd_day07_formula_fits_int(1024) || simd_day07_formula_fits_int(257)) {
        return fail("n=1024 不该被当成 int 公式安全");
    }
    const long long bound_256 = 256LL * 511 * 766;
    if (bound_256 != 100205056LL || bound_256 > 2147483647LL) {
        return fail("n=256 上界和 tests/README.md 不一致");
    }
    const long long max_a = 2LL * 1024 - 1;
    const long long max_b = 3LL * 1024 - 2;
    const long long bound_1024 = 1024LL * max_a * max_b;
    if (bound_1024 <= 2147483647LL) {
        return fail("n=1024 的保守上界没有超出 32 位有符号整数，拒绝开跑的理由不成立");
    }
    const std::string tag = simd_build_tag();
    if (tag != "V0" && tag != "V1" && tag != "V2") {
        return fail("build 标记不是 V0/V1/V2");
    }
#if defined(NO_VEC)
    if (tag != "V0") {
        return fail("定义了 NO_VEC 时标记应是 V0");
    }
#elif defined(__AVX2__)
    if (tag != "V2") {
        return fail("有 __AVX2__ 时标记应是 V2");
    }
#else
    if (tag != "V1") {
        return fail("默认编译的标记应是 V1。这只说明宏，不是 MSVC 的 SSE2 实测");
    }
#endif
    const std::string header = kSimdCsvHeader;
    if (header.find("build") == std::string::npos) {
        return fail("CSV 表头没有 build 列");
    }
    return true;
}

}  // namespace

int main() {
    using Ikj = void (*)(int*, int*, int*, int);
    using Blocked = void (*)(int*, int*, int*, int, int);
    Ikj ikj = &matmul_ikj_simd;
    Blocked blocked = &matmul_blocked_simd;
    if (ikj == nullptr || blocked == nullptr) {
        std::cerr << "FAIL: kernel 符号不存在\n";
        return 1;
    }
    if (!check_bounds()) {
        return 1;
    }
    if (!kSimdKernelImplemented) {
        std::cout
            << "SKIP: kernel 未实现（TODO(零醛)：matmul_ikj_simd、matmul_blocked_simd）。"
               "这不是 PASS。\n"
            << "build 标记是 " << simd_build_tag()
            << "。n=1024 的 int 公式上界超出 32 位，脚手架不另造初始化。\n"
            << "计时和与 day07 的逐元素比对还没执行。\n";
        return 0;
    }
    std::cout << "PASS: 标志已打开。本文件仍不在这里代跑计时。\n";
    return 0;
}
