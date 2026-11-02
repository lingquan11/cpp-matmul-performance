// 软件输出驻留脉动阵列周期模型：脚手架。
// 脚手架由 AI 维护；标 TODO(零醛) 的函数由零醛本人实现，不是 AI 代写的学生实现。
//
// 这是什么：命令行、CSV、ikj 对照和周期解析式。三个核心函数只有签名，调用时打印「未实现」并退出。
// 谁写的：AI 搭脚手架。拍推进、skew、PE 乘加不是这份文件里的实现。
//
// 编译（在仓库根目录，Linux / macOS）：
//   bash scripts/build-systolic-os-model.sh
// 或一条命令：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/systolic-os-model templates/gpt/systolic-os-model.cpp
// Windows x64 Native Tools（未在本次云端环境运行）：
//   cl /nologo /std:c++17 /EHsc /W4 /utf-8 templates\gpt\systolic-os-model.cpp /Fe:build\systolic-os-model.exe
//
// 运行：
//   ./build/systolic-os-model --n 2 --P 2 --trace
//   ./build/systolic-os-model --n 4 --P 2 --csv build/model.csv
// 核心未实现时不会创建 CSV。不要把本程序的运行耗时写进实验结果。
//
// 限制：不在 src/。实现前不要接入 tests/smoke.sh。容量 1..kMaxN、1..kMaxP。
// 不覆盖已有 CSV。解析式待零醛手算核对。

#ifndef SYSTOLIC_OS_MODEL_CPP_INCLUDED
#define SYSTOLIC_OS_MODEL_CPP_INCLUDED

#include "systolic-os-model.hpp"

#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

[[noreturn]] void unimplemented(const char* name) {
    std::cerr << "未实现: " << name
              << "（TODO(零醛)。脚手架没有写拍推进、输入错位或 PE 乘加。）\n";
    std::exit(1);
}

bool parse_positive(const std::string& text, int limit, int& value) {
    if (text.empty()) {
        return false;
    }
    value = 0;
    for (char ch : text) {
        if (ch < '0' || ch > '9') {
            return false;
        }
        const int digit = ch - '0';
        if (value > (limit - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
        if (value > limit) {
            return false;
        }
    }
    return value > 0;
}

void print_usage(std::ostream& out) {
    out << "用法: systolic-os-model --n <阶> --P <阵列边长> [--csv <路径>] [--trace]\n"
        << "软件输出驻留脉动阵列的周期模型脚手架。\n"
        << "step_one_cycle、inject_skewed_inputs、run_tile 未实现时，\n"
        << "程序打印「未实现」并以非零码退出，不写 CSV。\n"
        << "--trace 只用于 n=2 的逐拍表。\n"
        << "范围: 1 <= n <= " << kMaxN << "，1 <= P <= " << kMaxP << "。\n"
        << "这不是性能基准。运行耗时不要写入实验结果。\n";
}

}  // namespace

long long ceil_div_positive(int n, int P) {
    if (n <= 0 || P <= 0) {
        return -1;
    }
    return (static_cast<long long>(n) + P - 1) / P;
}

long long expected_tiles(int n, int P) {
    const long long side = ceil_div_positive(n, P);
    if (side < 0) {
        return -1;
    }
    return side * side;
}

long long expected_cycles(int n, int P) {
    const long long tiles = expected_tiles(n, P);
    if (tiles < 0) {
        return -1;
    }
    const long long per_tile = static_cast<long long>(n) + 2LL * static_cast<long long>(P) - 2;
    return tiles * per_tile;
}

bool prepare_model(SystolicModel& model, int n, int P, const char*& error) {
    error = nullptr;
    if (n <= 0 || n > kMaxN || P <= 0 || P > kMaxP) {
        error = "n 或 P 超出脚手架范围";
        return false;
    }
    model = SystolicModel{};
    model.n = n;
    model.P = P;
    model.pe.assign(static_cast<size_t>(P), std::vector<OsPe>(static_cast<size_t>(P)));
    model.left_a.assign(static_cast<size_t>(P), {});
    model.top_b.assign(static_cast<size_t>(P), {});
    model.A.assign(static_cast<size_t>(n), std::vector<int>(static_cast<size_t>(n), 0));
    model.B.assign(static_cast<size_t>(n), std::vector<int>(static_cast<size_t>(n), 0));
    model.C.assign(static_cast<size_t>(n), std::vector<long long>(static_cast<size_t>(n), 0));

    if (n == 2) {
        // 与 src/day02-matrix.cpp 相同的 Day 2 矩阵。不是新测量。
        const int day2_a[2][2] = {{2, 1}, {3, 4}};
        const int day2_b[2][2] = {{5, 2}, {1, 6}};
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                model.A[static_cast<size_t>(i)][static_cast<size_t>(j)] = day2_a[i][j];
                model.B[static_cast<size_t>(i)][static_cast<size_t>(j)] = day2_b[i][j];
            }
        }
    } else {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                model.A[static_cast<size_t>(i)][static_cast<size_t>(j)] = i + j + 1;
                model.B[static_cast<size_t>(i)][static_cast<size_t>(j)] = 2 * i + j + 1;
            }
        }
    }
    return true;
}

void matmul_ikj_reference(const std::vector<std::vector<int>>& A,
                          const std::vector<std::vector<int>>& B,
                          std::vector<std::vector<long long>>& C) {
    const int n = static_cast<int>(A.size());
    C.assign(static_cast<size_t>(n), std::vector<long long>(static_cast<size_t>(n), 0));
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            const long long a = A[static_cast<size_t>(i)][static_cast<size_t>(k)];
            for (int j = 0; j < n; ++j) {
                C[static_cast<size_t>(i)][static_cast<size_t>(j)] +=
                    a * B[static_cast<size_t>(k)][static_cast<size_t>(j)];
            }
        }
    }
}

bool same_as_ikj(const SystolicModel& model, const char*& why) {
    why = nullptr;
    std::vector<std::vector<long long>> reference;
    matmul_ikj_reference(model.A, model.B, reference);
    if (static_cast<int>(model.C.size()) != model.n) {
        why = "model.C 的行数不是 n";
        return false;
    }
    for (int i = 0; i < model.n; ++i) {
        if (static_cast<int>(model.C[static_cast<size_t>(i)].size()) != model.n) {
            why = "model.C 的列数不是 n";
            return false;
        }
        for (int j = 0; j < model.n; ++j) {
            if (model.C[static_cast<size_t>(i)][static_cast<size_t>(j)] !=
                reference[static_cast<size_t>(i)][static_cast<size_t>(j)]) {
                why = "model.C 与 ikj 参考不一致";
                return false;
            }
        }
    }
    return true;
}

bool write_model_csv(const char* path, const SystolicModel& model, const char*& error) {
    error = nullptr;
    if (path == nullptr || path[0] == '\0') {
        error = "csv 路径为空";
        return false;
    }
    std::ifstream existing(path);
    if (existing.good()) {
        error = "CSV 已存在，拒绝覆盖";
        return false;
    }
    if (model.cycles_done <= 0 || model.P <= 0) {
        error = "周期或 P 还不是正数，不写 CSV";
        return false;
    }
    const long long tiles = expected_tiles(model.n, model.P);
    const double util = static_cast<double>(model.macs) /
                        (static_cast<double>(model.cycles_done) * model.P * model.P);
    std::ostringstream line;
    line << model.n << ',' << model.P << ',' << tiles << ',' << model.cycles_done << ','
         << model.macs << ',' << std::fixed << std::setprecision(6) << util << ','
         << model.buf_reads_A << ',' << model.buf_reads_B << '\n';

    std::ofstream out(path);
    if (!out) {
        error = "打不开 CSV 路径";
        return false;
    }
    out << kCsvHeader << '\n' << line.str();
    if (!out) {
        error = "写入 CSV 失败";
        return false;
    }
    return true;
}

void print_cycle_snapshot(const SystolicModel& model) {
    const long long cycle_index = model.cycles_done - 1;
    std::cout << "cycle_index=" << cycle_index << " cycles_done=" << model.cycles_done << '\n';
    const int rows = static_cast<int>(model.pe.size());
    for (int i = 0; i < rows; ++i) {
        const int cols = static_cast<int>(model.pe[static_cast<size_t>(i)].size());
        for (int j = 0; j < cols; ++j) {
            const OsPe& pe = model.pe[static_cast<size_t>(i)][static_cast<size_t>(j)];
            std::cout << "PE(" << i << ',' << j << ") acc=" << pe.acc << " a=" << pe.a_in
                      << " b=" << pe.b_in << '\n';
        }
    }
}

void inject_skewed_inputs(SystolicModel& model, int tile_i, int tile_j) {
    // TODO(零醛)
    (void)model;
    (void)tile_i;
    (void)tile_j;
    unimplemented("inject_skewed_inputs");
}

void step_one_cycle(SystolicModel& model) {
    // TODO(零醛)
    (void)model;
    unimplemented("step_one_cycle");
}

void run_tile(SystolicModel& model, int tile_i, int tile_j) {
    // TODO(零醛)
    (void)model;
    (void)tile_i;
    (void)tile_j;
    unimplemented("run_tile");
}

void run_all_tiles(SystolicModel& model) {
    const long long side = ceil_div_positive(model.n, model.P);
    for (long long tile_i = 0; tile_i < side; ++tile_i) {
        for (long long tile_j = 0; tile_j < side; ++tile_j) {
            run_tile(model, static_cast<int>(tile_i), static_cast<int>(tile_j));
        }
    }
}

int main(int argc, char** argv) {
    int n = 0;
    int P = 0;
    bool have_n = false;
    bool have_p = false;
    bool trace = false;
    const char* csv_path = nullptr;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_usage(std::cout);
            return 0;
        }
        if (arg == "--trace") {
            trace = true;
            continue;
        }
        if (arg == "--n" || arg == "--P" || arg == "--csv") {
            if (i + 1 >= argc) {
                print_usage(std::cerr);
                return 2;
            }
            const std::string value = argv[++i];
            if (arg == "--csv") {
                csv_path = argv[i];
                continue;
            }
            int parsed = 0;
            const int limit = arg == "--n" ? kMaxN : kMaxP;
            if (!parse_positive(value, limit, parsed)) {
                std::cerr << "参数不是允许范围内的十进制正整数: " << arg << '\n';
                print_usage(std::cerr);
                return 2;
            }
            if (arg == "--n") {
                n = parsed;
                have_n = true;
            } else {
                P = parsed;
                have_p = true;
            }
            continue;
        }
        std::cerr << "不认识的参数: " << arg << '\n';
        print_usage(std::cerr);
        return 2;
    }

    if (!have_n || !have_p) {
        std::cerr << "必须同时给出 --n 和 --P。\n";
        print_usage(std::cerr);
        return 2;
    }
    if (trace && n != 2) {
        std::cerr << "--trace 只用于 n=2 的逐拍表。\n";
        return 2;
    }

    SystolicModel model;
    const char* error = nullptr;
    if (!prepare_model(model, n, P, error)) {
        std::cerr << (error != nullptr ? error : "prepare_model 失败") << '\n';
        return 2;
    }
    model.trace = trace;

    // 核心未实现时，run_tile 会在这里打印「未实现」并退出，不会写 CSV。
    run_all_tiles(model);

    const long long cycles = expected_cycles(n, P);
    if (model.cycles_done != cycles) {
        std::cerr << "周期与解析式不一致。模型 " << model.cycles_done << "，解析式 " << cycles
                  << "。解析式待零醛手算核对。\n";
        return 1;
    }
    const char* why = nullptr;
    if (!same_as_ikj(model, why)) {
        std::cerr << (why != nullptr ? why : "与 ikj 不一致") << '\n';
        return 1;
    }
    if (csv_path != nullptr) {
        if (!write_model_csv(csv_path, model, error)) {
            std::cerr << (error != nullptr ? error : "写 CSV 失败") << '\n';
            return 1;
        }
    }
    std::cout << "软件模型完成。n=" << n << " P=" << P << " cycles=" << model.cycles_done
              << "。这是周期计数，不是性能实测。\n";
    return 0;
}

#endif
