#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;

constexpr int MAX_N = 256;
constexpr int FORMAL_SAMPLES = 5;

// 不同编译器下的禁止内联标记，降低重复计算被内联优化的风险。
#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

// 保留固定容量数组，避免在计算过程中动态分配内存。
static int A[MAX_N][MAX_N] = {};
static int B[MAX_N][MAX_N] = {};
static int C[MAX_N][MAX_N] = {};

// 保存 ijk 版本计算出的参考结果
static int Reference[MAX_N][MAX_N] = {};

// 初始化输入矩阵：不计入正式计时。
void initialize_inputs(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }
}

 // 朴素矩阵乘法：i-j-k
 // C 清零和矩阵乘法都计入计时。
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
// 循环顺序优化版本：i-k-j
// C 清零和矩阵乘法都计入计时。
NOINLINE void matmul_ikj(int n) {
    // 第一步：清零结果矩阵，注意这部分也需要计时
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
        }
    }

    // 第二步：改变循环顺序
    for (int i = 0; i < n; ++i) {
        for (int k = 0; k < n; ++k) {
            const int aik = A[i][k];

            for (int j = 0; j < n; ++j) {
                C[i][j] += aik * B[k][j];/* TODO：使用 aik 和 B[k][j] */;
            }
        }
    }
}


// 在计时区间外计算校验和，使整个结果矩阵都被实际读取。
long long checksum_matrix(int n) {
    long long sum = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sum += C[i][j];
        }
    }

    return sum;
}

// 检查 C 是否与 Reference 相同，确保计算正确性。
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

// 根据 C[0][0] 的数学定义计算独立的预期值。
long long expected_c00(int n) {
    long long expected = 0;

    for (int k = 0; k < n; ++k) {
        expected += 1LL * (k + 1) * (2 * k + 1);
    }

    return expected;
}

// 根据试测结果选择重复次数，目标是每个样本约 100 ms。
// 设置上限，避免异常短的试测结果导致重复次数过大。
int choose_repeats(double pilot_ms) {
    constexpr double TARGET_SAMPLE_MS = 100.0;

    if (pilot_ms <= 0.0) {
        return 10000;
    }

    const double estimated =
        ceil(TARGET_SAMPLE_MS / pilot_ms);

    if (estimated < 1.0) {
        return 1;
    }

    if (estimated > 10000.0) {
        return 10000;
    }

    return static_cast<int>(estimated);
}


int main() {
    const int sizes[] = { 64, 128, 256 };

    // 使用 Day 5 专属文件，不覆盖 Day 4 数据。
    ofstream raw("matmul_benchmark_day5_loop_order.csv");

    if (!raw) {
        cerr << "错误：无法创建 Day 5 CSV 文件" << endl;
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

    for (int size_index = 0; size_index < 3; ++size_index) {
        const int n = sizes[size_index];

        // 输入初始化不计时。
        initialize_inputs(n);

        const long long expected = expected_c00(n);

        // =========================================
        // 第一阶段：运行 ijk，建立参考矩阵
        // =========================================

        const auto ijk_pilot_start =
            chrono::steady_clock::now();

        matmul_ijk(n);

        const auto ijk_pilot_end =
            chrono::steady_clock::now();

        const chrono::duration<double, milli> ijk_pilot_elapsed =
            ijk_pilot_end - ijk_pilot_start;

        const double pilot_ijk_ms = ijk_pilot_elapsed.count();

        if (C[0][0] != expected) {
            cerr << "错误：ijk 试测失败，n = "
                << n << endl;
            return 2;
        }

        // 保存 ijk 的完整结果，作为参考矩阵。
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                Reference[i][j] = C[i][j];
            }
        }

        const long long reference_checksum =
            checksum_matrix(n);

        // =========================================
        // 第二阶段：运行 ikj，验证参考结果
        // =========================================

        const auto ikj_pilot_start =
            chrono::steady_clock::now();

        matmul_ikj(n);

        const auto ikj_pilot_end =
            chrono::steady_clock::now();

        const chrono::duration<double, milli> ikj_pilot_elapsed =
            ikj_pilot_end - ikj_pilot_start;

        const double pilot_ikj_ms = ikj_pilot_elapsed.count();

        const long long ikj_pilot_checksum =
            checksum_matrix(n);

        if (C[0][0] != expected ||
            ikj_pilot_checksum != reference_checksum ||
            !matches_reference(n)) {

            cerr << "错误：ijk 与 ikj 的试测结果不一致，n = "
                << n << endl;
            return 2;
        }

        // 两种算法分别确定重复次数。
        const int repeats_ijk = choose_repeats(pilot_ijk_ms);
        const int repeats_ikj = choose_repeats(pilot_ikj_ms);

        cout << "\n开始正式测试，n = " << n << endl;
        cout << "ijk 重复次数：" << repeats_ijk << endl;
        cout << "ikj 重复次数：" << repeats_ikj << endl;

        // =========================================
        // 第三阶段：正式测量
        // =========================================

        // 奇数样本先测 ijk，偶数样本先测 ikj。
        // 交替测量顺序，减少固定先后顺序带来的偏差。

        auto run_sample = [&](const char* algorithm,
            auto matmul,
            int repeats,
            double pilot_ms,
            int sample) -> bool {

                const auto start =
                    chrono::steady_clock::now();

                for (int rep = 0; rep < repeats; ++rep) {
                    matmul(n);
                }

                const auto end =
                    chrono::steady_clock::now();

                const chrono::duration<double, milli> elapsed =
                    end - start;

                const double total_ms = elapsed.count();
                const double ms_per_multiply =
                    total_ms / repeats;

                // 以下检查全部在计时结束后执行。
                const long long checksum = checksum_matrix(n);

                const bool elementwise_match =
                    matches_reference(n);

                const bool passed =
                    (C[0][0] == expected) &&
                    (checksum == reference_checksum) &&
                    elementwise_match;

                const char* match_text =
                    elementwise_match ? "YES" : "NO";

                const char* status =
                    passed ? "PASS" : "FAIL";

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
                    cerr << "错误：正式样本验证失败，n = "
                        << n << ", algorithm = "
                        << algorithm
                        << ", sample = " << sample << endl;
                }

                return passed;
            };

        for (int sample = 1;
            sample <= FORMAL_SAMPLES;
            ++sample) {

            if (sample % 2 == 1) {
                // 奇数样本：先 ijk，再 ikj。
                if (!run_sample(
                    "ijk",
                    [](int size) { matmul_ijk(size); },
                    repeats_ijk, pilot_ijk_ms, sample)) {
                    return 2;
                }

                if (!run_sample(
                    "ikj",
                    [](int size) { matmul_ikj(size); },
                    repeats_ikj, pilot_ikj_ms, sample)) {
                    return 2;
                }
            }
            else {
                // 偶数样本：先 ikj，再 ijk。
                if (!run_sample(
                    "ikj",
                    [](int size) { matmul_ikj(size); },
                    repeats_ikj, pilot_ikj_ms, sample)) {
                    return 2;
                }

                if (!run_sample(
                    "ijk",
                    [](int size) { matmul_ijk(size); },
                    repeats_ijk, pilot_ijk_ms, sample)) {
                    return 2;
                }
            }
        }
    }

    cout << "\nDay 5 实验完成。"
        << "原始数据已写入 "
        << "matmul_benchmark_day5_loop_order.csv"
        << endl;

    return 0;
}