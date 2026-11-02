// 输出驻留脉动阵列脚手架的检查。测试适配由 AI 维护，不代替零醛实现核心。
//
// 这是什么：核心未实现时输出 SKIP。实现后核对 Day 2 逐拍、列出的周期和利用率、以及与 ikj 逐元素一致。
// 谁写的：AI。标 TODO(零醛) 的模拟不是这份测试代写的。
//
// 编译（仓库根目录）：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/systolic-model-check tests/systolic-model-check.cpp
// 或：bash scripts/build-systolic-os-model.sh
//
// 限制：不要把本文件接进 tests/smoke.sh，直到三个核心函数实现且 kSystolicCoreImplemented 改为 true。
// 待零醛实现后接入。SKIP 不是 PASS。本测试不写 CSV，也不计时。
// 列出的 4、24、4992、50%、约 66.7% 都标了待零醛手算核对。手算表若与这里的拍次不一致，以手算表为准，再改本文件。

#define main systolic_os_model_main
#include "../templates/gpt/systolic-os-model.cpp"
#undef main

#include <iostream>
#include <string>

namespace {

bool fail(const std::string& message) {
    std::cerr << "FAIL: " << message << '\n';
    return false;
}

bool listed_formula_ok() {
    // 待零醛手算核对。这里只检查解析式函数是否等于提示词列出的三个整数。
    if (expected_cycles(2, 2) != 4) {
        return fail("expected_cycles(2,2) 不是列出的 4");
    }
    if (expected_cycles(4, 2) != 24) {
        return fail("expected_cycles(4,2) 不是列出的 24");
    }
    if (expected_cycles(64, 8) != 4992) {
        return fail("expected_cycles(64,8) 不是列出的 4992");
    }
    if (expected_tiles(2, 2) != 1 || expected_tiles(4, 2) != 4 || expected_tiles(64, 8) != 64) {
        return fail("expected_tiles 与 (ceil(n/P))^2 不一致");
    }
    const std::string header = kCsvHeader;
    if (header != "n,P,tiles,cycles,macs,util,buf_reads_A,buf_reads_B") {
        return fail("CSV 表头与约定列不一致");
    }
    return true;
}

bool day2_oracle_ok() {
    SystolicModel model;
    const char* error = nullptr;
    if (!prepare_model(model, 2, 2, error)) {
        return fail(error != nullptr ? error : "prepare_model(2,2) 失败");
    }
    const int expect_a[2][2] = {{2, 1}, {3, 4}};
    const int expect_b[2][2] = {{5, 2}, {1, 6}};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (model.A[static_cast<size_t>(i)][static_cast<size_t>(j)] != expect_a[i][j] ||
                model.B[static_cast<size_t>(i)][static_cast<size_t>(j)] != expect_b[i][j]) {
                return fail("n=2 的输入不是 Day 2 矩阵");
            }
        }
    }
    std::vector<std::vector<long long>> C;
    matmul_ikj_reference(model.A, model.B, C);
    const long long expect_c[2][2] = {{11, 10}, {19, 30}};
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            if (C[static_cast<size_t>(i)][static_cast<size_t>(j)] != expect_c[i][j]) {
                return fail("Day 2 的 ikj oracle 不是 11 10 / 19 30");
            }
        }
    }
    if (model.cycles_done != 0 || model.macs != 0) {
        return fail("prepare_model 不应提前计入周期或 MAC");
    }
    return true;
}

bool expect_cycles(const SystolicModel& model, long long want, const std::string& label) {
    if (model.cycles_done != want) {
        return fail(label + " cycles_done 不符合");
    }
    if (model.cycles_done != expected_cycles(model.n, model.P)) {
        return fail(label + " 周期与解析式不一致");
    }
    return true;
}

bool check_day2_trace() {
    // 待零醛手算核对。
    // 拍次从 0 起：PE(i,j) 在 t = i+j+k 收到第 k 对。K=2 时最终值出现在 t = i+j+1。
    // 因此 t=1 时 PE(0,0)=11；t=2 时 PE(0,1)=10 且 PE(1,0)=19；t=3 时 PE(1,1)=30。
    // 提示词写的「第 1 拍 / 第 2 拍 / 第 3 拍」按这个 t 理解。手算若不同，改这里。
    SystolicModel model;
    const char* error = nullptr;
    if (!prepare_model(model, 2, 2, error)) {
        return fail(error != nullptr ? error : "prepare_model 失败");
    }
    inject_skewed_inputs(model, 0, 0);
    const int steps = 2 + 2 * 2 - 2;  // K+2P-2，K=2，P=2，列出的 4 拍
    if (steps != 4) {
        return fail("单 tile 4 拍的常量写错了");
    }
    for (int t = 0; t < steps; ++t) {
        step_one_cycle(model);
        if (model.cycles_done != static_cast<long long>(t) + 1) {
            return fail("step_one_cycle 没有把 cycles_done 加 1");
        }
        const OsPe& pe00 = model.pe[0][0];
        const OsPe& pe01 = model.pe[0][1];
        const OsPe& pe10 = model.pe[1][0];
        const OsPe& pe11 = model.pe[1][1];
        if (t == 1 && pe00.acc != 11) {
            return fail("t=1 时 PE(0,0) 不是 11");
        }
        if (t == 2 && (pe01.acc != 10 || pe10.acc != 19)) {
            return fail("t=2 时 PE(0,1)/PE(1,0) 不是 10 和 19");
        }
        if (t == 3 && pe11.acc != 30) {
            return fail("t=3 时 PE(1,1) 不是 30");
        }
    }
    return true;
}

bool check_case(int n, int P, long long want_cycles, bool check_exact_util) {
    SystolicModel model;
    const char* error = nullptr;
    if (!prepare_model(model, n, P, error)) {
        return fail(error != nullptr ? error : "prepare_model 失败");
    }
    run_all_tiles(model);
    if (!expect_cycles(model, want_cycles, "n=" + std::to_string(n) + " P=" + std::to_string(P))) {
        return false;
    }
    if (model.buf_reads_A < 0 || model.buf_reads_B < 0) {
        return fail("缓冲读计数是负数");
    }
    const char* why = nullptr;
    if (!same_as_ikj(model, why)) {
        return fail(why != nullptr ? why : "与 ikj 不一致");
    }
    if (n == 2 && P == 2) {
        const long long expect_c[2][2] = {{11, 10}, {19, 30}};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (model.C[static_cast<size_t>(i)][static_cast<size_t>(j)] != expect_c[i][j]) {
                    return fail("n=2,P=2 的 C 不是 Day 2 结果");
                }
            }
        }
    }
    if (check_exact_util) {
        // 利用率 = macs / (cycles * P * P)。待零醛手算核对。
        // n=2,P=2：列出 50%，且 2、4 整除，有用乘加 n^3=8，8/(4*4)=1/2。
        // n=4,P=2：列出约 66.7%，4、2 整除，有用乘加 64，64/(24*4)=2/3。
        const long long denom = model.cycles_done * model.P * model.P;
        const long long useful = static_cast<long long>(n) * n * n;
        if (model.macs != useful) {
            return fail("macs 不是 n^3（整除情形下的有用乘加）");
        }
        if (n == 2 && useful * 2 != denom) {
            return fail("2x2 利用率不是 50%");
        }
        if (n == 4 && useful * 3 != denom * 2) {
            return fail("n=4,P=2 利用率不是 2/3（约 66.7%）");
        }
    }
    return true;
}

bool implemented_checks() {
    if (!check_day2_trace()) {
        return false;
    }
    if (!check_case(2, 2, 4, true)) {
        return false;
    }
    if (!check_case(4, 2, 24, true)) {
        return false;
    }
    if (!check_case(64, 8, 4992, false)) {
        return false;
    }
    const int sizes[] = {8, 64, 70};
    const int array_sizes[] = {2, 4, 8};
    for (int n : sizes) {
        for (int P : array_sizes) {
            const long long cycles = expected_cycles(n, P);
            if (!check_case(n, P, cycles, false)) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace

int main() {
    if (!listed_formula_ok() || !day2_oracle_ok()) {
        return 1;
    }
    if (!kSystolicCoreImplemented) {
        std::cout
            << "SKIP: 核心未实现（TODO(零醛)：step_one_cycle、inject_skewed_inputs、run_tile）。"
               "这不是 PASS。\n"
            << "解析式与提示词列出的 4、24、4992 一致，Day 2 的 ikj oracle 得到 11 10 / 19 30。"
               "这是整数算术和参考三重循环，不是脉动模拟输出，待零醛手算核对。\n"
            << "逐拍表、利用率、以及 n 与 ikj 的对账尚未执行。\n";
        return 0;
    }
    if (!implemented_checks()) {
        return 1;
    }
    std::cout << "PASS: Day 2 逐拍、列出的周期与利用率、以及 n∈{8,64,70}×P∈{2,4,8} 的 ikj "
                 "对账均已执行。软件模型计数，不是性能实测。\n";
    return 0;
}
