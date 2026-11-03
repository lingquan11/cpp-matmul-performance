// SSE2 与 AVX2 的计时对照脚手架。
// 脚手架由 AI 维护；标 TODO(零醛) 的函数由零醛本人实现，不是 AI 代写的学生实现。
//
// 这是什么：给 ikj、blocked_16、blocked_64 准备同一套参数和 CSV。
//           V1 / V2 的差别在编译命令，不在这里复制两份 kernel。
//           V0 是扩展：编译时加 -DNO_VEC，内层循环前放下面的宏。
// 谁写的：AI 搭参数、计时和 CSV。kernel 函数体只有 TODO(零醛)。
//
// 编译（仓库根目录）：
//   bash scripts/build-lda-simd.sh
// Windows x64 Native Tools 的交替编译在 scripts\run-simd-factor.bat。
//   V1：cl /nologo /O2 /EHsc /utf-8 templates\gpt\simd-factor.cpp /Fe:build\simd-factor-v1.exe
//   V2：同一条加上 /arch:AVX2
//   V0（扩展，不在四周交替里）：再加 /DNO_VEC
// 本云端环境没有 cl，上面三条没有在这里跑过。
//
// 运行：
//   ./build/simd-factor --algo ikj --n 256 --runs 5 --out build/simd-factor.csv
// 核心未实现时打印「未实现」并以非零码退出，不创建 --out。
//
// 限制：本文件不在 src/。不要把 day07 的函数体复制进来。
// n=256 时若决定复用 day07，在测试里写：
//   #define main day07_blocking_main
//   #include "../../src/day07-matrix-blocking.cpp"
//   #undef main
// n=1024 超过 day07 的 MAX_N=256，那条路走不通。
// day07 的 A=i+j+1、B=2*i+j+1 只在 n<=256 时保证 int 累加不溢出
// （tests/README.md 的上界 256*511*766=100205056）。
// n=1024 的同一上界是 1024*(2*1024-1)*(3*1024-2)，大于 32 位有符号上限。
// 脚手架不另造公式。标志改成 true 之后，n=1024 仍会拒绝开跑，直到零醛自己定初始化。
//
// 计时口径与仓库一致：计时含 C 清零和乘法；初始化、打印、正确性检查在外。
// 正式计时依赖尚未合并的分支 ideas/matmul-1002-i1-measurement-baseline
// 里的 experiments/reports/measurement-rules.md（规则 R）。本文件不复制那份草案。
// g++ 默认指令集不是 MSVC x64 的 SSE2。这里印出的 V1 只表示「没有 __AVX2__ 也没有 NO_VEC」。
// 云端或 Linux 的毫秒不能写入正式 CSV 或报告。
// 不要为了让 vec-report 变绿而改 day07。V1 接近 V2 本身也是允许的结果，不是失败。

#ifndef SIMD_FACTOR_CPP_INCLUDED
#define SIMD_FACTOR_CPP_INCLUDED

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#if defined(NO_VEC) && defined(_MSC_VER)
#define SIMD_NO_VECTOR __pragma(loop(no_vector))
#else
#define SIMD_NO_VECTOR
#endif

// TODO(零醛): kernel 实现并核对之后，把下一行改成 true。
// false 时：测试输出 SKIP；main 打印「未实现」并以非零码退出，不写 CSV。
inline constexpr bool kSimdKernelImplemented = false;

inline constexpr int kSimdRunsDefault = 5;

inline constexpr char kSimdCsvHeader[] =
    "n,algorithm,build,pilot_ms,sample,repeats,total_ms,ms_per_multiply,"
    "C00,expected_C00,checksum,elementwise_match,status";

struct SimdRequest {
    std::string algo;
    int n = 0;
    int block_size = 0;
    int runs = kSimdRunsDefault;
    std::string out;
};

const char* simd_build_tag() {
#if defined(NO_VEC)
    return "V0";
#elif defined(__AVX2__)
    return "V2";
#else
    return "V1";
#endif
}

// n<=256 才允许沿用 day07 的 int 公式。这是溢出边界，不是计时结果。
bool simd_day07_formula_fits_int(int n) {
    return n >= 1 && n <= 256;
}

[[noreturn]] void simd_unimplemented(const char* name) {
    std::cerr << "未实现: " << name
              << "（TODO(零醛)。脚手架没有写 ikj 或分块 kernel。）\n";
    std::exit(1);
}

// TODO(零醛): ikj。行跨度就是 n，本文件不做 LDA 实验。
// 清零 C 的工作放在函数里面，算进计时。
// 写内层循环时把 SIMD_NO_VECTOR 放在 for 的上一行。这个宏在 MSVC 且定义了 NO_VEC 时
// 才是 #pragma loop(no_vector)；g++ 下它是空的，这样本文件才能被 smoke 以外的 g++ 编译。
// 出处：https://learn.microsoft.com/en-us/cpp/preprocessor/loop
void matmul_ikj_simd(int* A, int* B, int* C, int n) {
    (void)A;
    (void)B;
    (void)C;
    (void)n;
    // SIMD_NO_VECTOR
    // for (int j = 0; j < n; ++j) { ... }
    simd_unimplemented("matmul_ikj_simd");
}

// TODO(零醛): 块内仍是 ikj。block_size 由 --algo 决定：blocked_16 是 16，blocked_64 是 64。
// 不要把 day07 的 matmul_blocked 复制到这里。
void matmul_blocked_simd(int* A, int* B, int* C, int n, int block_size) {
    (void)A;
    (void)B;
    (void)C;
    (void)n;
    (void)block_size;
    // SIMD_NO_VECTOR
    // for (int j = jj; j < j_end; ++j) { ... }
    simd_unimplemented("matmul_blocked_simd");
}

namespace {

void print_usage(std::ostream& out) {
    out << "用法: simd-factor --algo ikj|blocked_16|blocked_64 --n 256|1024 "
           "[--runs <样本数>] --out <csv>\n"
        << "SIMD 对照脚手架。build 标记是 " << simd_build_tag()
        << "（V0=NO_VEC，V2=__AVX2__，其余记成 V1）。\n"
        << "kernel 未实现时打印「未实现」并以非零码退出，不写 CSV。\n"
        << "n=1024 的初始化还没定，实现 kernel 之后这一档仍会拒绝开跑。\n"
        << "这不是已经完成的测量。不要把 AVX2 预先写成一定快于 SSE2。\n";
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
    }
    return value > 0 && value <= limit;
}

bool parse_request(int argc, char** argv, SimdRequest& request, bool& help, std::string& error) {
    help = false;
    bool got_algo = false;
    bool got_n = false;
    bool got_out = false;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto need = [&](std::string& slot) -> bool {
            if (i + 1 >= argc) {
                error = "缺少参数值: " + arg;
                return false;
            }
            slot = argv[++i];
            return true;
        };
        if (arg == "--help" || arg == "-h") {
            help = true;
            return true;
        }
        if (arg == "--algo") {
            std::string value;
            if (!need(value)) {
                return false;
            }
            if (value == "ikj") {
                request.algo = value;
                request.block_size = 0;
            } else if (value == "blocked_16") {
                request.algo = value;
                request.block_size = 16;
            } else if (value == "blocked_64") {
                request.algo = value;
                request.block_size = 64;
            } else {
                error = "--algo 只能是 ikj、blocked_16 或 blocked_64";
                return false;
            }
            got_algo = true;
        } else if (arg == "--n") {
            std::string value;
            if (!need(value)) {
                return false;
            }
            if (!parse_positive(value, 1024, request.n) ||
                (request.n != 256 && request.n != 1024)) {
                error = "--n 只能是 256 或 1024";
                return false;
            }
            got_n = true;
        } else if (arg == "--runs") {
            std::string value;
            if (!need(value)) {
                return false;
            }
            if (!parse_positive(value, 100000, request.runs)) {
                error = "--runs 必须是正整数";
                return false;
            }
        } else if (arg == "--out") {
            if (!need(request.out)) {
                return false;
            }
            got_out = true;
        } else {
            error = "无法识别的参数: " + arg;
            return false;
        }
    }
    if (!got_algo || !got_n || !got_out) {
        error = "必须同时给出 --algo、--n 和 --out";
        return false;
    }
    return true;
}

[[maybe_unused]] bool file_exists(const std::string& path) {
    std::ifstream in(path);
    return static_cast<bool>(in);
}

[[maybe_unused]] void fill_day07_inputs(std::vector<int>& A, std::vector<int>& B, int n) {
    A.assign(static_cast<size_t>(n * n), 0);
    B.assign(static_cast<size_t>(n * n), 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[static_cast<size_t>(i * n + j)] = i + j + 1;
            B[static_cast<size_t>(i * n + j)] = 2 * i + j + 1;
        }
    }
}

[[maybe_unused]] void reference_i64(const std::vector<int>& A, const std::vector<int>& B, int n,
                                  std::vector<long long>& out) {
    out.assign(static_cast<size_t>(n * n), 0);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            const long long aik = A[static_cast<size_t>(i * n + k)];
            for (int j = 0; j < n; ++j) {
                out[static_cast<size_t>(i * n + j)] +=
                    aik * static_cast<long long>(B[static_cast<size_t>(k * n + j)]);
            }
        }
    }
}

[[maybe_unused]] int choose_repeats(double pilot_ms) {
    constexpr double target_ms = 100.0;
    if (!(pilot_ms > 0.0)) {
        return 10000;
    }
    const double estimated = target_ms / pilot_ms;
    int repeats = static_cast<int>(estimated);
    if (static_cast<double>(repeats) < estimated) {
        ++repeats;
    }
    if (repeats < 1) {
        return 1;
    }
    if (repeats > 10000) {
        return 10000;
    }
    return repeats;
}

[[maybe_unused]] int run_simd(const SimdRequest& request) {
    if (!simd_day07_formula_fits_int(request.n)) {
        std::cerr << "n=" << request.n
                  << " 不能沿用 day07 的 int 初始化。乘加上界会超出 32 位有符号整数。"
                     "脚手架不另造公式。TODO(零醛): 先定元素类型或初始化，再跑这一档。不写 CSV。\n";
        return 1;
    }
    if (file_exists(request.out)) {
        std::cerr << "拒绝覆盖已有文件: " << request.out << "\n";
        return 2;
    }

    std::vector<int> A;
    std::vector<int> B;
    fill_day07_inputs(A, B, request.n);
    std::vector<int> C(static_cast<size_t>(request.n * request.n), 7);
    std::vector<long long> reference;
    reference_i64(A, B, request.n, reference);
    const long long expected_c00 = reference[0];
    long long expected_sum = 0;
    for (long long value : reference) {
        expected_sum += value;
    }

    const auto once = [&](std::vector<int>& target) {
        if (request.algo == "ikj") {
            matmul_ikj_simd(A.data(), B.data(), target.data(), request.n);
        } else {
            matmul_blocked_simd(A.data(), B.data(), target.data(), request.n, request.block_size);
        }
    };

    const auto pilot_start = std::chrono::steady_clock::now();
    once(C);
    const auto pilot_end = std::chrono::steady_clock::now();
    const double pilot_ms =
        std::chrono::duration<double, std::milli>(pilot_end - pilot_start).count();

    bool pilot_ok = true;
    for (int index = 0; index < request.n * request.n; ++index) {
        if (C[static_cast<size_t>(index)] != reference[static_cast<size_t>(index)]) {
            pilot_ok = false;
            break;
        }
    }
    if (!pilot_ok) {
        std::cerr << "试测与 int64 参考不一致，不写 CSV。\n";
        return 1;
    }

    const int repeats = choose_repeats(pilot_ms);
    std::ofstream raw(request.out);
    if (!raw) {
        std::cerr << "无法创建 CSV: " << request.out << "\n";
        return 1;
    }
    raw << std::fixed << std::setprecision(6);
    raw << kSimdCsvHeader << '\n';
    const char* build = simd_build_tag();

    for (int sample = 1; sample <= request.runs; ++sample) {
        std::fill(C.begin(), C.end(), 7);
        const auto start = std::chrono::steady_clock::now();
        for (int rep = 0; rep < repeats; ++rep) {
            once(C);
        }
        const auto end = std::chrono::steady_clock::now();
        const double total_ms =
            std::chrono::duration<double, std::milli>(end - start).count();
        const double ms_per = total_ms / static_cast<double>(repeats);
        long long checksum = 0;
        bool match = true;
        for (int index = 0; index < request.n * request.n; ++index) {
            checksum += C[static_cast<size_t>(index)];
            if (C[static_cast<size_t>(index)] != reference[static_cast<size_t>(index)]) {
                match = false;
            }
        }
        const bool passed = match && static_cast<long long>(C[0]) == expected_c00 &&
                            checksum == expected_sum;
        raw << request.n << ',' << request.algo << ',' << build << ',' << pilot_ms << ','
            << sample << ',' << repeats << ',' << total_ms << ',' << ms_per << ',' << C[0]
            << ',' << expected_c00 << ',' << checksum << ',' << (match ? "YES" : "NO") << ','
            << (passed ? "PASS" : "FAIL") << '\n';
        raw.flush();
        if (!passed) {
            std::cerr << "正式样本与 int64 参考不一致，已写下 FAIL 行。\n";
            return 1;
        }
    }
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    SimdRequest request;
    bool help = false;
    std::string error;
    if (!parse_request(argc, argv, request, help, error)) {
        if (!error.empty()) {
            std::cerr << error << "\n";
        }
        print_usage(std::cerr);
        return 2;
    }
    if (help) {
        print_usage(std::cout);
        return 0;
    }
    if (!kSimdKernelImplemented) {
        std::cerr << "未实现: " << request.algo << "（TODO(零醛)）。已解析 n=" << request.n
                  << " block=" << request.block_size << " build=" << simd_build_tag()
                  << " runs=" << request.runs << "。不写 CSV。\n";
        if (request.n == 1024) {
            std::cerr << "另外：n=1024 不能沿用 day07 的 int 初始化公式，乘加会溢出。"
                         "公式待零醛定，脚手架不另造。\n";
        }
        return 1;
    }
    return run_simd(request);
}

#endif
