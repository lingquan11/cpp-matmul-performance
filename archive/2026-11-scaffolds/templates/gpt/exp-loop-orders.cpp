// 六种循环顺序的计时脚手架。
//
// 它是什么：复用 Day 7 的 matmul_ijk、matmul_ikj，并为 jik、kij、jki、kji
// 留下签名。参数是 --algo --n --runs --out。
// 谁写的：脚手架由 AI 维护；标 TODO(零醛) 的函数由零醛本人实现，不是 AI 代写的学生实现。
// 怎么编译和运行（在仓库根目录）：
//   g++ -std=c++17 -O2 -o build/exp-loop-orders templates/gpt/exp-loop-orders.cpp
//   build/exp-loop-orders --algo ikj --n 64 --runs 1 --out build/ikj-format.csv
// 限制：
// - 本文件放在 templates/gpt/，不放进 src/。tests/smoke.sh 不会编译它。
// - jik、kij、jki、kji 未实现。调用时向 stderr 写「未实现」并以退出码 1 结束，
//   不写 CSV，也不改写一个假的 C。
// - 正式毫秒只由零醛在自己的 Windows 电脑上测量。这里若跑出毫秒，只许用来看格式，
//   不得写入 experiments/results/ 或正式报告。
// - 不含 SCALE-Sim。循环顺序类比于哪种数据流，留给纸笔表，本文件不填。
//
// 正确性测试把 EXP_LOOP_ORDERS_NO_MAIN 设上再包含本文件，从而不跑本入口。

#include <chrono>
#include <climits>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>

#define main day07_benchmark_main
#include "../../src/day07-matrix-blocking.cpp"
#undef main

class NotImplementedError : public std::runtime_error {
public:
    explicit NotImplementedError(const char* message) : std::runtime_error(message) {}
};

// TODO(零醛): 实现 j-i-k（字母自外向内）。脚手架不写循环体。
void matmul_jik(int n) {
    (void)n;
    throw NotImplementedError("未实现：matmul_jik（TODO(零醛)）");
}

// TODO(零醛): 实现 k-i-j（字母自外向内）。脚手架不写循环体。
void matmul_kij(int n) {
    (void)n;
    throw NotImplementedError("未实现：matmul_kij（TODO(零醛)）");
}

// TODO(零醛): 实现 j-k-i（字母自外向内）。脚手架不写循环体。
void matmul_jki(int n) {
    (void)n;
    throw NotImplementedError("未实现：matmul_jki（TODO(零醛)）");
}

// TODO(零醛): 实现 k-j-i（字母自外向内）。脚手架不写循环体。
void matmul_kji(int n) {
    (void)n;
    throw NotImplementedError("未实现：matmul_kji（TODO(零醛)）");
}

struct LoopOrder {
    const char* name;
    void (*fn)(int);
};

void call_ijk(int n) { matmul_ijk(n); }
void call_ikj(int n) { matmul_ikj(n); }

#ifndef EXP_LOOP_ORDERS_NO_MAIN
namespace {

const LoopOrder kOrders[] = {
    {"ijk", call_ijk},
    {"ikj", call_ikj},
    {"jik", matmul_jik},
    {"kij", matmul_kij},
    {"jki", matmul_jki},
    {"kji", matmul_kji},
};

std::int64_t oracle[MAX_N][MAX_N];

void usage() {
    std::cerr
        << "用法: exp-loop-orders --algo ijk|ikj|jik|kij|jki|kji --n N --runs R --out FILE\n"
        << "n 为 1..256 的十进制整数。runs 为 1..100000 的十进制整数，是本进程内的重复次数。\n"
        << "退出码：0 成功；1 未实现或结果与 int64 参考不一致；2 参数错误。\n"
        << "--help 也以退出码 2 结束，并打印这段用法。\n"
        << "未实现时不写 FILE。拒绝把 FILE 放进 experiments/results/。\n"
        << "计时含该 kernel 自己的 C 清零和乘法；初始化与正确性检查在计时外。\n";
}

bool parse_positive(const char* text, int limit, int& value) {
    if (text == nullptr || text[0] == '\0') {
        return false;
    }
    value = 0;
    for (const char* p = text; *p != '\0'; ++p) {
        if (*p < '0' || *p > '9') {
            return false;
        }
        const int digit = *p - '0';
        if (value > (limit - digit) / 10) {
            return false;
        }
        value = value * 10 + digit;
    }
    return value > 0;
}

bool archived_results_path(std::string path) {
    for (char& ch : path) {
        if (ch == '\\') {
            ch = '/';
        }
    }
    const std::string padded = "/" + path + "/";
    return padded.find("/experiments/results/") != std::string::npos;
}

// 独立 int64 参考，用来拒绝假结果。它不是六种循环顺序里的新 kernel，也不进入计时。
void fill_oracle(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            std::int64_t sum = 0;
            for (int k = 0; k < n; ++k) {
                sum += static_cast<std::int64_t>(A[i][k]) * static_cast<std::int64_t>(B[k][j]);
            }
            oracle[i][j] = sum;
        }
    }
}

bool matches_oracle(int n, long long& checksum) {
    checksum = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            const std::int64_t expected = oracle[i][j];
            if (expected < INT_MIN || expected > INT_MAX || C[i][j] != expected) {
                return false;
            }
            checksum += C[i][j];
        }
    }
    return true;
}

const LoopOrder* find_order(const char* name) {
    for (const LoopOrder& order : kOrders) {
        if (std::string_view(order.name) == name) {
            return &order;
        }
    }
    return nullptr;
}

}  // namespace

int main(int argc, char** argv) {
    const char* algo = nullptr;
    const char* out_path = nullptr;
    int n = 0;
    int runs = 0;
    bool saw_n = false;
    bool saw_runs = false;

    for (int i = 1; i < argc; ++i) {
        const std::string_view arg = argv[i];
        auto need = [&](const char* flag) -> const char* {
            if (i + 1 >= argc) {
                std::cerr << "缺少 " << flag << " 的值。\n";
                usage();
                return nullptr;
            }
            return argv[++i];
        };
        if (arg == "--help" || arg == "-h") {
            usage();
            return 2;
        }
        if (arg == "--algo") {
            algo = need("--algo");
            if (algo == nullptr) {
                return 2;
            }
        } else if (arg == "--n") {
            const char* text = need("--n");
            if (text == nullptr || !parse_positive(text, MAX_N, n)) {
                std::cerr << "n 必须是 1..256 的十进制整数。\n";
                usage();
                return 2;
            }
            saw_n = true;
        } else if (arg == "--runs") {
            const char* text = need("--runs");
            if (text == nullptr || !parse_positive(text, 100000, runs)) {
                std::cerr << "runs 必须是 1..100000 的十进制整数。\n";
                usage();
                return 2;
            }
            saw_runs = true;
        } else if (arg == "--out") {
            out_path = need("--out");
            if (out_path == nullptr) {
                return 2;
            }
        } else {
            std::cerr << "无法识别的参数：" << argv[i] << "\n";
            usage();
            return 2;
        }
    }

    if (algo == nullptr || out_path == nullptr || !saw_n || !saw_runs) {
        usage();
        return 2;
    }
    if (out_path[0] == '\0' || archived_results_path(out_path)) {
        std::cerr << "拒绝写入。--out 不能是空路径，也不能放进 experiments/results/。\n";
        return 2;
    }

    const LoopOrder* order = find_order(algo);
    if (order == nullptr) {
        std::cerr << "未知 algo。允许：ijk ikj jik kij jki kji。\n";
        usage();
        return 2;
    }

    initialize_inputs(n);
    fill_oracle(n);

    try {
        order->fn(n);
    } catch (const NotImplementedError& ex) {
        std::cerr << ex.what() << "\n";
        std::cerr << "未写输出文件。\n";
        return 1;
    }

    long long checksum = 0;
    if (!matches_oracle(n, checksum)) {
        std::cerr << "错误：计时前的结果与 int64 参考不一致，n=" << n
                  << " algo=" << order->name << "。不写 CSV。\n";
        return 1;
    }

    const auto start = std::chrono::steady_clock::now();
    try {
        for (int rep = 0; rep < runs; ++rep) {
            order->fn(n);
        }
    } catch (const NotImplementedError& ex) {
        std::cerr << ex.what() << "\n";
        std::cerr << "未写输出文件。\n";
        return 1;
    }
    const auto end = std::chrono::steady_clock::now();
    const double total_ms = std::chrono::duration<double, std::milli>(end - start).count();
    const double ms_per_multiply = total_ms / static_cast<double>(runs);

    long long timed_checksum = 0;
    if (!matches_oracle(n, timed_checksum) || timed_checksum != checksum) {
        std::cerr << "错误：计时后的结果与 int64 参考不一致，n=" << n
                  << " algo=" << order->name << "。不写 CSV。\n";
        return 1;
    }

    std::ofstream raw(out_path);
    if (!raw) {
        std::cerr << "错误：无法写入 " << out_path << "\n";
        return 1;
    }
    raw << std::fixed << std::setprecision(6);
    raw << "n,algorithm,runs,total_ms,ms_per_multiply,C00,checksum,elementwise_match,status\n";
    raw << n << ',' << order->name << ',' << runs << ',' << total_ms << ','
        << ms_per_multiply << ',' << C[0][0] << ',' << checksum << ",YES,PASS\n";
    if (!raw) {
        std::cerr << "错误：写入 " << out_path << " 时失败。\n";
        return 1;
    }
    std::cout << "已写入一行格式检查用 CSV：" << out_path
              << "。若本机不是零醛的 Windows 正式环境，不要把毫秒收进报告。\n";
    return 0;
}
#endif
