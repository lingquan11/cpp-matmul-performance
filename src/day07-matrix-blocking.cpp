// Day 7 分块矩阵乘法。
//
// 来源标签：根据学习对话整理的 GPT 整合版。
// 这不是用户本机逐字源码，也不是用户在对话里粘贴的中间版。
// 用户中间版仍在 code_history/day07/V01_user_submitted_intermediate.cpp，本文件不替换它。
// GPT 片段仍在 templates/gpt/day07-blocking-snippets.md。
//
// 归档生成环境曾用下面这条命令编译并运行：边界测试通过，75 条正式样本全部 PASS。
//   g++ -std=c++17 -O2 -Wall -Wextra -pedantic
// 没有在用户本机 Visual Studio 重新构建。本次合并若在临时目录复跑，
// 只用于核对这份整合版能否编译运行，不能写成用户 CSV 的复现，
// 也不能覆盖 experiments/results/2026-10-27-day07-blocking-local/ 下的正式 CSV。
//
// 仓库 src/ 用连字符命名，故本文件是 day07-matrix-blocking.cpp，
// 对应补丁内建议路径 src/day07_matrix_blocking.cpp。

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>

using namespace std;

constexpr int MAX_N = 256;
constexpr int FORMAL_SAMPLES = 5;
constexpr int BLOCK_SIZE_COUNT = 3;
constexpr int ALGORITHM_COUNT = 5;

// 不同编译器下的禁止内联标记，降低重复计算被内联优化的风险。
#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

// 固定容量数组，避免在计算过程中动态分配内存。
static int A[MAX_N][MAX_N] = {};
static int B[MAX_N][MAX_N] = {};
static int C[MAX_N][MAX_N] = {};
static int Reference[MAX_N][MAX_N] = {};

// 初始化输入矩阵，不计入正式计时。
void initialize_inputs(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }
}

// 基线版本：i-j-k。
// C 清零与矩阵乘法均计入函数调用的计时。
NOINLINE void matmul_ijk(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

// 循环顺序优化版本：i-k-j。
// C 清零与矩阵乘法均计时。
NOINLINE void matmul_ikj(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            const int aik = A[i][k];
            for (int j = 0; j < n; ++j) {
                C[i][j] += aik * B[k][j];
            }
        }
    }
}

// 分块矩阵乘法：外层按 ii/jj/kk 分块，块内使用 i-k-j。
// C 清零只执行一次，且计入计时。
NOINLINE void matmul_blocked(int n, int block_size) {
    // 调用方使用的 block_size 必须为正数。
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
        }
    }

    for (int ii = 0; ii < n; ii += block_size) {
        const int i_end = min(ii + block_size, n);

        for (int jj = 0; jj < n; jj += block_size) {
            const int j_end = min(jj + block_size, n);

            for (int kk = 0; kk < n; kk += block_size) {
                const int k_end = min(kk + block_size, n);

                for (int i = ii; i < i_end; ++i) {
                    for (int k = kk; k < k_end; ++k) {
                        const int aik = A[i][k];
                        for (int j = jj; j < j_end; ++j) {
                            C[i][j] += aik * B[k][j];
                        }
                    }
                }
            }
        }
    }
}

// 在计时区间外计算校验和。
long long checksum_matrix(int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sum += C[i][j];
        }
    }
    return sum;
}

// 逐元素比较当前结果矩阵与参考矩阵。
bool matches_reference(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (C[i][j] != Reference[i][j]) {
                return false;
            }
        }
    }
    return true;
}

// 根据 C[0][0] 的数学定义计算独立预期值。
long long expected_c00(int n) {
    long long expected = 0;
    for (int k = 0; k < n; ++k) {
        expected += 1LL * (k + 1) * (2 * k + 1);
    }
    return expected;
}

// 根据试测耗时选择重复次数，目标每个正式样本约 100 ms。
int choose_repeats(double pilot_ms) {
    constexpr double TARGET_SAMPLE_MS = 100.0;

    if (pilot_ms <= 0.0) {
        return 10000;
    }

    const double estimated = ceil(TARGET_SAMPLE_MS / pilot_ms);

    if (estimated < 1.0) {
        return 1;
    }
    if (estimated > 10000.0) {
        return 10000;
    }
    return static_cast<int>(estimated);
}

// 独立边界测试：验证 n 不能被 block_size 整除的情况。
// 此测试不计入正式计时，也不写入正式性能 CSV。
bool run_boundary_test() {
    constexpr int test_n = 70;
    constexpr int test_block_size = 32;

    initialize_inputs(test_n);

    matmul_ijk(test_n);
    if (C[0][0] != expected_c00(test_n)) {
        cerr << "错误：边界测试的 ijk 参考结果异常。" << endl;
        return false;
    }

    for (int i = 0; i < test_n; ++i) {
        for (int j = 0; j < test_n; ++j) {
            Reference[i][j] = C[i][j];
        }
    }

    const long long reference_sum = checksum_matrix(test_n);

    matmul_blocked(test_n, test_block_size);

    const bool passed =
        C[0][0] == expected_c00(test_n) &&
        checksum_matrix(test_n) == reference_sum &&
        matches_reference(test_n);

    cout << "边界测试 n=70, block_size=32: "
         << (passed ? "PASS" : "FAIL") << endl;

    return passed;
}

int main() {
    const int sizes[] = { 64, 128, 256 };
    const int block_sizes[BLOCK_SIZE_COUNT] = { 16, 32, 64 };

    // 使用 Day 7 专属文件，避免覆盖 Day 4、Day 5 数据。
    ofstream raw("matmul_benchmark_day7_blocking.csv");

    if (!raw) {
        cerr << "错误：无法创建 Day 7 CSV 文件。" << endl;
        cerr << "请检查当前工作目录、写入权限以及文件是否被占用。"
             << endl;
        return 1;
    }

    cout << fixed << setprecision(6);
    raw << fixed << setprecision(6);

    const char* header =
        "n,algorithm,pilot_ms,sample,repeats,total_ms,"
        "ms_per_multiply,C00,expected_C00,checksum,"
        "elementwise_match,status";

    cout << header << '\n';
    raw << header << '\n';
    raw.flush();

    // 在正式性能测试前单独验证不完整分块。
    // 该测试不写入正式 CSV 数据行。
    if (!run_boundary_test()) {
        cerr << "错误：分块边界测试失败。" << endl;
        return 2;
    }

    for (int size_index = 0; size_index < 3; ++size_index) {
        const int n = sizes[size_index];
        initialize_inputs(n);

        const long long expected = expected_c00(n);

        // 第一阶段：运行 ijk，生成参考矩阵。
        const auto ijk_pilot_start = chrono::steady_clock::now();
        matmul_ijk(n);
        const auto ijk_pilot_end = chrono::steady_clock::now();

        const chrono::duration<double, milli> ijk_pilot_elapsed =
            ijk_pilot_end - ijk_pilot_start;
        const double pilot_ijk_ms = ijk_pilot_elapsed.count();

        if (C[0][0] != expected) {
            cerr << "错误：ijk 试测失败，n = " << n << endl;
            return 2;
        }

        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                Reference[i][j] = C[i][j];
            }
        }

        const long long reference_checksum = checksum_matrix(n);

        // 第二阶段：运行 ikj，并与 ijk 的参考矩阵比较。
        const auto ikj_pilot_start = chrono::steady_clock::now();
        matmul_ikj(n);
        const auto ikj_pilot_end = chrono::steady_clock::now();

        const chrono::duration<double, milli> ikj_pilot_elapsed =
            ikj_pilot_end - ikj_pilot_start;
        const double pilot_ikj_ms = ikj_pilot_elapsed.count();
        const long long ikj_pilot_checksum = checksum_matrix(n);

        if (C[0][0] != expected ||
            ikj_pilot_checksum != reference_checksum ||
            !matches_reference(n)) {
            cerr << "错误：ijk 与 ikj 的试测结果不一致，n = "
                 << n << endl;
            return 2;
        }

        // 第三阶段：分别试测三种分块大小。
        double pilot_blocked_ms[BLOCK_SIZE_COUNT] = {};
        int repeats_blocked[BLOCK_SIZE_COUNT] = {};

        for (int b = 0; b < BLOCK_SIZE_COUNT; ++b) {
            const int block_size = block_sizes[b];

            const auto blocked_pilot_start = chrono::steady_clock::now();
            matmul_blocked(n, block_size);
            const auto blocked_pilot_end = chrono::steady_clock::now();

            const chrono::duration<double, milli> blocked_elapsed =
                blocked_pilot_end - blocked_pilot_start;
            pilot_blocked_ms[b] = blocked_elapsed.count();

            const long long blocked_checksum = checksum_matrix(n);

            if (C[0][0] != expected ||
                blocked_checksum != reference_checksum ||
                !matches_reference(n)) {
                cerr << "错误：分块试测失败，n = " << n
                     << ", block_size = " << block_size << endl;
                return 2;
            }

            repeats_blocked[b] = choose_repeats(pilot_blocked_ms[b]);
        }

        const int repeats_ijk = choose_repeats(pilot_ijk_ms);
        const int repeats_ikj = choose_repeats(pilot_ikj_ms);

        cout << "\n开始正式测试，n = " << n << endl;
        cout << "ijk 重复次数：" << repeats_ijk << endl;
        cout << "ikj 重复次数：" << repeats_ikj << endl;

        for (int b = 0; b < BLOCK_SIZE_COUNT; ++b) {
            cout << "blocked_" << block_sizes[b]
                 << " 重复次数：" << repeats_blocked[b] << endl;
        }

        // 每次正式样本统一使用这一个函数完成计时、校验与 CSV 输出。
        auto run_sample = [&](const char* algorithm,
                              auto matmul,
                              int repeats,
                              double pilot_ms,
                              int sample) -> bool {
            const auto start = chrono::steady_clock::now();

            for (int rep = 0; rep < repeats; ++rep) {
                matmul(n);
            }

            const auto end = chrono::steady_clock::now();
            const chrono::duration<double, milli> elapsed = end - start;

            const double total_ms = elapsed.count();
            const double ms_per_multiply = total_ms / repeats;

            // 正确性检查均在计时区间之外。
            const long long checksum = checksum_matrix(n);
            const bool elementwise_match = matches_reference(n);

            const bool passed =
                C[0][0] == expected &&
                checksum == reference_checksum &&
                elementwise_match;

            const char* match_text = elementwise_match ? "YES" : "NO";
            const char* status = passed ? "PASS" : "FAIL";

            cout << n << ','
                 << algorithm << ','
                 << pilot_ms << ','
                 << sample << ','
                 << repeats << ','
                 << total_ms << ','
                 << ms_per_multiply << ','
                 << C[0][0] << ','
                 << expected << ','
                 << checksum << ','
                 << match_text << ','
                 << status << '\n';

            raw << n << ','
                << algorithm << ','
                << pilot_ms << ','
                << sample << ','
                << repeats << ','
                << total_ms << ','
                << ms_per_multiply << ','
                << C[0][0] << ','
                << expected << ','
                << checksum << ','
                << match_text << ','
                << status << '\n';
            raw.flush();

            if (!passed) {
                cerr << "错误：正式样本验证失败，n = " << n
                     << ", algorithm = " << algorithm
                     << ", sample = " << sample << endl;
            }

            return passed;
        };

        // 根据算法编号调度五种算法配置。
        auto run_algorithm_sample = [&](int algorithm_index,
                                        int sample) -> bool {
            switch (algorithm_index) {
            case 0:
                return run_sample(
                    "ijk",
                    [](int size) { matmul_ijk(size); },
                    repeats_ijk, pilot_ijk_ms, sample);

            case 1:
                return run_sample(
                    "ikj",
                    [](int size) { matmul_ikj(size); },
                    repeats_ikj, pilot_ikj_ms, sample);

            case 2:
                return run_sample(
                    "blocked_16",
                    [](int size) { matmul_blocked(size, 16); },
                    repeats_blocked[0], pilot_blocked_ms[0], sample);

            case 3:
                return run_sample(
                    "blocked_32",
                    [](int size) { matmul_blocked(size, 32); },
                    repeats_blocked[1], pilot_blocked_ms[1], sample);

            case 4:
                return run_sample(
                    "blocked_64",
                    [](int size) { matmul_blocked(size, 64); },
                    repeats_blocked[2], pilot_blocked_ms[2], sample);

            default:
                return false;
            }
        };

        // 五次正式样本轮换算法起始顺序，减少固定顺序造成的部分偏差。
        for (int sample = 1; sample <= FORMAL_SAMPLES; ++sample) {
            for (int offset = 0; offset < ALGORITHM_COUNT; ++offset) {
                const int algorithm_index =
                    (sample - 1 + offset) % ALGORITHM_COUNT;

                if (!run_algorithm_sample(algorithm_index, sample)) {
                    return 2;
                }
            }
        }
    }

    cout << "\nDay 7 分块实验完成。原始数据已写入 "
         << "matmul_benchmark_day7_blocking.csv" << endl;

    return 0;
}
