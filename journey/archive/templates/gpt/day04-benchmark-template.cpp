// GPT 模板，非我的代码。
// 此文件是此前教学中提供的基准测试模板，不保证与我电脑上实际执行的版本逐字一致。

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

void initialize_inputs(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }
}

NOINLINE void matmul(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;

            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

long long checksum_matrix(int n) {
    long long sum = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            sum += C[i][j];
        }
    }

    return sum;
}

long long expected_c00(int n) {
    long long expected = 0;

    for (int k = 0; k < n; ++k) {
        expected += 1LL * (k + 1) * (2 * k + 1);
    }

    return expected;
}

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
    const int sizes[] = {64, 128, 256};

    ofstream raw("matmul_benchmark_raw.csv");

    if (!raw) {
        cerr << "错误：无法创建 matmul_benchmark_raw.csv" << endl;
        return 1;
    }

    cout << fixed << setprecision(6);
    raw << fixed << setprecision(6);

    const char* header =
        "n,pilot_ms,sample,repeats,total_ms,"
        "ms_per_multiply,C00,expected_C00,checksum,status";

    cout << header << '\n';
    raw << header << '\n';

    for (int size_index = 0; size_index < 3; ++size_index) {
        const int n = sizes[size_index];

        initialize_inputs(n);

        const auto pilot_start =
            chrono::steady_clock::now();

        matmul(n);

        const auto pilot_end =
            chrono::steady_clock::now();

        const chrono::duration<double, milli> pilot_elapsed =
            pilot_end - pilot_start;

        const double pilot_ms = pilot_elapsed.count();

        const long long pilot_checksum = checksum_matrix(n);
        const long long expected = expected_c00(n);

        if (C[0][0] != expected) {
            cerr << "错误：试测正确性检查失败，n = "
                 << n << endl;
            return 2;
        }

        const int repeats = choose_repeats(pilot_ms);

        for (int sample = 1;
             sample <= FORMAL_SAMPLES;
             ++sample) {

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

            const long long checksum = checksum_matrix(n);

            const bool passed =
                (C[0][0] == expected) &&
                (checksum == pilot_checksum);

            const char* status = passed ? "PASS" : "FAIL";

            cout << n << ','
                 << pilot_ms << ','
                 << sample << ','
                 << repeats << ','
                 << total_ms << ','
                 << ms_per_multiply << ','
                 << C[0][0] << ','
                 << expected << ','
                 << checksum << ','
                 << status << '\n';

            raw << n << ','
                << pilot_ms << ','
                << sample << ','
                << repeats << ','
                << total_ms << ','
                << ms_per_multiply << ','
                << C[0][0] << ','
                << expected << ','
                << checksum << ','
                << status << '\n';

            raw.flush();

            if (!passed) {
                cerr << "错误：正式样本校验失败，n = "
                     << n << ", sample = " << sample << endl;
                return 2;
            }
        }
    }

    cout << "\n实验完成。原始数据已写入 "
         << "matmul_benchmark_raw.csv" << endl;

    return 0;
}
