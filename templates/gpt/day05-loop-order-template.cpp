// GPT 指导模板：i-j-k 与 i-k-j 循环顺序对比。
// 说明：这是教学参考实现，不是对用户电脑上实际源文件逐字一致性的声明。
// 建议使用 Visual Studio C++14 或更新标准，Release | x64。

#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cmath>

using namespace std;

constexpr int MAX_N = 256;
constexpr int FORMAL_SAMPLES = 5;

#if defined(_MSC_VER)
#define NOINLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__clang__)
#define NOINLINE __attribute__((noinline))
#else
#define NOINLINE
#endif

static int A[MAX_N][MAX_N] = {};
static int B[MAX_N][MAX_N] = {};
static int C[MAX_N][MAX_N] = {};
static int Reference[MAX_N][MAX_N] = {};

void initialize_inputs(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }
}

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

long long checksum_matrix(int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            sum += C[i][j];
    return sum;
}

long long expected_c00(int n) {
    long long expected = 0;
    for (int k = 0; k < n; ++k)
        expected += 1LL * (k + 1) * (2 * k + 1);
    return expected;
}

bool matches_reference(int n) {
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (C[i][j] != Reference[i][j])
                return false;
    return true;
}

int choose_repeats(double pilot_ms) {
    constexpr double TARGET_SAMPLE_MS = 100.0;
    if (pilot_ms <= 0.0)
        return 10000;
    const double estimated = ceil(TARGET_SAMPLE_MS / pilot_ms);
    if (estimated < 1.0)
        return 1;
    if (estimated > 10000.0)
        return 10000;
    return static_cast<int>(estimated);
}

int main() {
    const int sizes[] = {64, 128, 256};
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
        initialize_inputs(n);
        const long long expected = expected_c00(n);

        const auto ijk_start = chrono::steady_clock::now();
        matmul_ijk(n);
        const auto ijk_end = chrono::steady_clock::now();
        const chrono::duration<double, milli> ijk_elapsed = ijk_end - ijk_start;
        const double pilot_ijk_ms = ijk_elapsed.count();

        if (C[0][0] != expected) {
            cerr << "错误：ijk 试测失败，n = " << n << endl;
            return 2;
        }

        for (int i = 0; i < n; ++i)
            for (int j = 0; j < n; ++j)
                Reference[i][j] = C[i][j];

        const long long reference_checksum = checksum_matrix(n);

        const auto ikj_start = chrono::steady_clock::now();
        matmul_ikj(n);
        const auto ikj_end = chrono::steady_clock::now();
        const chrono::duration<double, milli> ikj_elapsed = ikj_end - ikj_start;
        const double pilot_ikj_ms = ikj_elapsed.count();

        if (C[0][0] != expected ||
            checksum_matrix(n) != reference_checksum ||
            !matches_reference(n)) {
            cerr << "错误：ijk 与 ikj 的试测结果不一致，n = " << n << endl;
            return 2;
        }

        const int repeats_ijk = choose_repeats(pilot_ijk_ms);
        const int repeats_ikj = choose_repeats(pilot_ikj_ms);

        auto run_sample = [&](const char* algorithm, auto multiply,
                              int repeats, double pilot_ms, int sample) -> bool {
            const auto start = chrono::steady_clock::now();
            for (int rep = 0; rep < repeats; ++rep)
                multiply(n);
            const auto end = chrono::steady_clock::now();

            const chrono::duration<double, milli> elapsed = end - start;
            const double total_ms = elapsed.count();
            const double ms_per_multiply = total_ms / repeats;
            const long long checksum = checksum_matrix(n);
            const bool elementwise_match = matches_reference(n);
            const bool passed = C[0][0] == expected &&
                                checksum == reference_checksum &&
                                elementwise_match;

            const char* match_text = elementwise_match ? "YES" : "NO";
            const char* status = passed ? "PASS" : "FAIL";

            cout << n << ',' << algorithm << ',' << pilot_ms << ','
                 << sample << ',' << repeats << ',' << total_ms << ','
                 << ms_per_multiply << ',' << C[0][0] << ','
                 << expected << ',' << checksum << ','
                 << match_text << ',' << status << '\n';
            raw << n << ',' << algorithm << ',' << pilot_ms << ','
                << sample << ',' << repeats << ',' << total_ms << ','
                << ms_per_multiply << ',' << C[0][0] << ','
                << expected << ',' << checksum << ','
                << match_text << ',' << status << '\n';
            raw.flush();

            if (!passed) {
                cerr << "错误：正式样本验证失败，n = " << n
                     << ", algorithm = " << algorithm
                     << ", sample = " << sample << endl;
            }
            return passed;
        };

        for (int sample = 1; sample <= FORMAL_SAMPLES; ++sample) {
            if (sample % 2 == 1) {
                if (!run_sample("ijk", [](int x) { matmul_ijk(x); },
                                repeats_ijk, pilot_ijk_ms, sample))
                    return 2;
                if (!run_sample("ikj", [](int x) { matmul_ikj(x); },
                                repeats_ikj, pilot_ikj_ms, sample))
                    return 2;
            } else {
                if (!run_sample("ikj", [](int x) { matmul_ikj(x); },
                                repeats_ikj, pilot_ikj_ms, sample))
                    return 2;
                if (!run_sample("ijk", [](int x) { matmul_ijk(x); },
                                repeats_ijk, pilot_ijk_ms, sample))
                    return 2;
            }
        }
    }

    cout << "\nDay 5 实验完成。原始数据已写入 "
         << "matmul_benchmark_day5_loop_order.csv" << endl;
    return 0;
}
