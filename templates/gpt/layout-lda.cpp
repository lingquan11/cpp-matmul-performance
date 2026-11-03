// 行跨度（LDA）实验脚手架。
// 脚手架由 AI 维护；标 TODO(零醛) 的函数由零醛本人实现，不是 AI 代写的学生实现。
//
// 这是什么：把 day07 固定的 int[256][256]（行跨度 1024 字节）换成运行时参数 lda。
//           同一份机器码跑 lda = n、n+16、256。ijk_Bt 不在最小版里，下面只留注释，不建函数。
// 谁写的：AI 搭参数、计时、CSV 和对账用的 int64 参考。matmul_ijk_lda / matmul_ikj_lda 只有签名。
//
// 编译（仓库根目录，Linux / macOS）：
//   bash scripts/build-lda-simd.sh
// 或：
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/layout-lda templates/gpt/layout-lda.cpp
// Windows x64 Native Tools（本云端环境没有 cl，下面这条没有在这里跑过）：
//   cl /nologo /std:c++17 /EHsc /O2 /utf-8 templates\gpt\layout-lda.cpp /Fe:build\layout-lda.exe
//
// 运行：
//   ./build/layout-lda --algo ijk --n 64 --lda n --runs 5 --out build/layout-lda.csv
//   ./build/layout-lda --algo ikj --n 256 --lda n+16 --runs 5 --out build/layout-lda.csv
// 核心未实现时打印「未实现」并以非零码退出，不创建 --out。
//
// 限制：本文件不在 src/。tests/smoke.sh 会编译 src/ 下全部 cpp，实现前不要拷进去。
// 初始化沿用 day07：A=i+j+1，B=2*i+j+1。n<=256 时，保守上界
// 256*511*766=100205056，小于 32 位有符号上限。这个上界写在 tests/README.md，不是新测的耗时。
// 计时口径与仓库一致：计时含 C 清零和乘法；初始化、打印、正确性检查在外。
// 正式计时依赖尚未合并的分支 ideas/matmul-1002-i1-measurement-baseline
// 里的 experiments/reports/measurement-rules.md（规则 R）。本文件不复制那份草案。
// 云端或 Linux 的毫秒不能写入正式 CSV 或报告。

#ifndef LAYOUT_LDA_CPP_INCLUDED
#define LAYOUT_LDA_CPP_INCLUDED

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

// TODO(零醛): 两个 kernel 实现并核对之后，把下一行改成 true。
// false 时：测试输出 SKIP，不输出 PASS；main 打印「未实现」并以非零码退出，不写 CSV。
inline constexpr bool kLayoutLdaImplemented = false;

inline constexpr int kLayoutMaxN = 256;
inline constexpr int kLayoutMaxLda = 1024;
inline constexpr int kLayoutDefaultRuns = 5;

inline constexpr char kLayoutCsvHeader[] =
    "n,lda,algorithm,pilot_ms,sample,repeats,total_ms,ms_per_multiply,"
    "C00,expected_C00,checksum,elementwise_match,status";

// 可选对照是先把 B 转置再跑 ijk（ijk_Bt）。最小版不建这个函数，避免多一份机器码。

struct LayoutRequest {
    std::string algo;
    int n = 0;
    int lda = 0;
    int runs = kLayoutDefaultRuns;
    std::string out;
};

[[noreturn]] void layout_unimplemented(const char* name) {
    std::cerr << "未实现: " << name
              << "（TODO(零醛)。脚手架没有写带 lda 的矩阵乘法。）\n";
    std::exit(1);
}

// 元素 (i, j) 在下标 i * lda + j。lda 是运行时参数，不进类型，也不复制多份循环。
inline int layout_index(int i, int lda, int j) {
    return i * lda + j;
}

// 正整数，不允许符号、空格和前导加号。超过 limit 视为失败。
bool layout_parse_positive(const std::string& text, int limit, int& value) {
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

// token 为 n、n+16，或直接的十进制 lda。成功时 lda >= n 且 lda <= kLayoutMaxLda。
bool layout_resolve_lda(const std::string& token, int n, int& lda, std::string& error) {
    if (n <= 0 || n > kLayoutMaxN) {
        error = "n 必须先落在 1..256，才能解释 lda";
        return false;
    }
    if (token == "n") {
        lda = n;
    } else if (token == "n+16") {
        lda = n + 16;
    } else if (!layout_parse_positive(token, kLayoutMaxLda, lda)) {
        error = "lda 只能写成 n、n+16，或 1..1024 的整数";
        return false;
    }
    if (lda < n) {
        error = "lda 小于 n，这一行放不下 n 个元素";
        return false;
    }
    if (lda > kLayoutMaxLda) {
        error = "lda 超过脚手架上限 1024";
        return false;
    }
    return true;
}

// day07 的公式，只填逻辑上的 n×n。lda 多出来的列保持 0，kernel 不应把它们当输入。
void layout_fill_inputs(int* A, int* B, int n, int lda) {
    const int cells = n * lda;
    for (int index = 0; index < cells; ++index) {
        A[index] = 0;
        B[index] = 0;
    }
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[layout_index(i, lda, j)] = i + j + 1;
            B[layout_index(i, lda, j)] = 2 * i + j + 1;
        }
    }
}

// 对账用的 int64 参考，不是被计时的 kernel。只读 j < n、k < n，行距用 lda。
void layout_reference_i64(const int* A, const int* B, int n, int lda,
                          std::vector<long long>& out) {
    out.assign(static_cast<size_t>(n * n), 0);
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            const long long aik = A[layout_index(i, lda, k)];
            for (int j = 0; j < n; ++j) {
                out[static_cast<size_t>(i * n + j)] +=
                    aik * static_cast<long long>(B[layout_index(k, lda, j)]);
            }
        }
    }
}

// TODO(零醛): ijk，lda 只从参数进来。所有 LDA 水平都必须调用这一份函数。
// 元素 (i,j) 在 i*lda+j。函数内部清零 C 的 n×n 逻辑区域，清零算进计时。
// 调用前 C 被填成 7，用来发现没写到的格子。不要按 lda 复制出第二份循环。
void matmul_ijk_lda(int* A, int* B, int* C, int n, int lda) {
    (void)A;
    (void)B;
    (void)C;
    (void)n;
    (void)lda;
    layout_unimplemented("matmul_ijk_lda");
}

// TODO(零醛): ikj，约束与 matmul_ijk_lda 相同，仍然是这一份机器码吃不同的 lda。
void matmul_ikj_lda(int* A, int* B, int* C, int n, int lda) {
    (void)A;
    (void)B;
    (void)C;
    (void)n;
    (void)lda;
    layout_unimplemented("matmul_ikj_lda");
}

namespace {

void print_usage(std::ostream& out) {
    out << "用法: layout-lda --algo ijk|ikj --n <阶> --lda n|n+16|<整数> "
           "[--runs <样本数>] --out <csv>\n"
        << "行跨度实验脚手架。n 的实验档是 64、128、256；1..256 的其它阶只给正确性抽查。\n"
        << "matmul_ijk_lda、matmul_ikj_lda 未实现时，程序打印「未实现」并以非零码退出，不写 CSV。\n"
        << "lda 必须 >= n，并且 <= " << kLayoutMaxLda << "。\n"
        << "这不是已经完成的测量。耗时不要写入正式报告。\n";
}

bool parse_request(int argc, char** argv, LayoutRequest& request, bool& help, std::string& error) {
    help = false;
    bool got_algo = false;
    bool got_n = false;
    bool got_lda = false;
    bool got_out = false;
    bool got_runs = false;
    std::string lda_token;
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
            if (value != "ijk" && value != "ikj") {
                error = "--algo 只能是 ijk 或 ikj";
                return false;
            }
            request.algo = value;
            got_algo = true;
        } else if (arg == "--n") {
            std::string value;
            if (!need(value)) {
                return false;
            }
            if (!layout_parse_positive(value, kLayoutMaxN, request.n)) {
                error = "--n 必须是 1..256 的整数";
                return false;
            }
            got_n = true;
        } else if (arg == "--lda") {
            if (!need(lda_token)) {
                return false;
            }
            got_lda = true;
        } else if (arg == "--runs") {
            std::string value;
            if (!need(value)) {
                return false;
            }
            if (!layout_parse_positive(value, 100000, request.runs)) {
                error = "--runs 必须是正整数";
                return false;
            }
            got_runs = true;
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
    if (!got_algo || !got_n || !got_lda || !got_out) {
        error = "必须同时给出 --algo、--n、--lda 和 --out";
        return false;
    }
    if (!got_runs) {
        request.runs = kLayoutDefaultRuns;
    }
    return layout_resolve_lda(lda_token, request.n, request.lda, error);
}

[[maybe_unused]] bool file_exists(const std::string& path) {
    std::ifstream in(path);
    return static_cast<bool>(in);
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

using Kernel = void (*)(int*, int*, int*, int, int);

[[maybe_unused]] Kernel kernel_for(const std::string& algo) {
    if (algo == "ijk") {
        return &matmul_ijk_lda;
    }
    return &matmul_ikj_lda;
}

[[maybe_unused]] int run_layout(const LayoutRequest& request) {
    if (file_exists(request.out)) {
        std::cerr << "拒绝覆盖已有文件: " << request.out << "\n";
        return 2;
    }

    const int cells = request.n * request.lda;
    std::vector<int> A(static_cast<size_t>(cells));
    std::vector<int> B(static_cast<size_t>(cells));
    std::vector<int> C(static_cast<size_t>(cells), 7);
    layout_fill_inputs(A.data(), B.data(), request.n, request.lda);

    std::vector<long long> reference;
    layout_reference_i64(A.data(), B.data(), request.n, request.lda, reference);
    const long long expected_c00 = reference[0];
    long long expected_sum = 0;
    for (long long value : reference) {
        expected_sum += value;
    }

    Kernel kernel = kernel_for(request.algo);
    const auto pilot_start = std::chrono::steady_clock::now();
    kernel(A.data(), B.data(), C.data(), request.n, request.lda);
    const auto pilot_end = std::chrono::steady_clock::now();
    const double pilot_ms =
        std::chrono::duration<double, std::milli>(pilot_end - pilot_start).count();

    bool pilot_ok = true;
    for (int i = 0; i < request.n && pilot_ok; ++i) {
        for (int j = 0; j < request.n; ++j) {
            if (C[static_cast<size_t>(layout_index(i, request.lda, j))] !=
                reference[static_cast<size_t>(i * request.n + j)]) {
                pilot_ok = false;
                break;
            }
        }
    }
    if (!pilot_ok) {
        std::cerr << "试测与 int64 参考不一致，不写 CSV。n=" << request.n
                  << " lda=" << request.lda << " algo=" << request.algo << "\n";
        return 1;
    }

    const int repeats = choose_repeats(pilot_ms);
    std::ofstream raw(request.out);
    if (!raw) {
        std::cerr << "无法创建 CSV: " << request.out << "\n";
        return 1;
    }
    raw << std::fixed << std::setprecision(6);
    raw << kLayoutCsvHeader << '\n';

    for (int sample = 1; sample <= request.runs; ++sample) {
        std::fill(C.begin(), C.end(), 7);
        const auto start = std::chrono::steady_clock::now();
        for (int rep = 0; rep < repeats; ++rep) {
            kernel(A.data(), B.data(), C.data(), request.n, request.lda);
        }
        const auto end = std::chrono::steady_clock::now();
        const double total_ms =
            std::chrono::duration<double, std::milli>(end - start).count();
        const double ms_per = total_ms / static_cast<double>(repeats);

        long long checksum = 0;
        bool match = true;
        for (int i = 0; i < request.n; ++i) {
            for (int j = 0; j < request.n; ++j) {
                const int value = C[static_cast<size_t>(layout_index(i, request.lda, j))];
                checksum += value;
                if (value != reference[static_cast<size_t>(i * request.n + j)]) {
                    match = false;
                }
            }
        }
        const long long c00 = C[static_cast<size_t>(layout_index(0, request.lda, 0))];
        const bool passed = match && c00 == expected_c00 && checksum == expected_sum;
        raw << request.n << ',' << request.lda << ',' << request.algo << ','
            << pilot_ms << ',' << sample << ',' << repeats << ',' << total_ms << ','
            << ms_per << ',' << c00 << ',' << expected_c00 << ',' << checksum << ','
            << (match ? "YES" : "NO") << ',' << (passed ? "PASS" : "FAIL") << '\n';
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
    LayoutRequest request;
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
    if (!kLayoutLdaImplemented) {
        std::cerr << "未实现: matmul_" << request.algo
                  << "_lda（TODO(零醛)）。已解析 n=" << request.n
                  << " lda=" << request.lda << " algo=" << request.algo
                  << " runs=" << request.runs << "。不写 CSV。\n";
        return 1;
    }
    return run_layout(request);
}

#endif
