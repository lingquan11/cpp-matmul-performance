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

// 初始化输入矩阵：不计入正式计时。
void initialize_inputs(int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }
}

// 朴素矩阵乘法。
// C 清零和矩阵乘法都在此函数中，因此都计入计时。
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

    // 原始数据写入当前工作目录中的 CSV 文件。
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

        // 输入初始化不计时。
        initialize_inputs(n);

        // 试测：只用于决定正式测量的重复次数。
        const auto pilot_start =
            chrono::steady_clock::now();

        matmul_ikj(n);

        const auto pilot_end =
            chrono::steady_clock::now();

        const chrono::duration<double, milli> pilot_elapsed =
            pilot_end - pilot_start;

        const double pilot_ms = pilot_elapsed.count();

        // 试测结果在计时后检查。
        const long long pilot_checksum = checksum_matrix(n);
        const long long expected = expected_c00(n);

        if (C[0][0] != expected) {
            cerr << "错误：试测正确性检查失败，n = "
                << n << endl;
            return 2;
        }

        const int repeats = choose_repeats(pilot_ms);

        // 每种规模进行五次正式测量。
        for (int sample = 1;
            sample <= FORMAL_SAMPLES;
            ++sample) {

            const auto start =
                chrono::steady_clock::now();

            for (int rep = 0; rep < repeats; ++rep) {
                matmul_ikj(n);
            }

            const auto end =
                chrono::steady_clock::now();

            const chrono::duration<double, milli> elapsed =
                end - start;

            const double total_ms = elapsed.count();
            const double ms_per_multiply =
                total_ms / repeats;

            // 正确性检查、校验和计算均在计时结束后执行。
            const long long checksum = checksum_matrix(n);

            const bool passed =
                (C[0][0] == expected) &&
                (checksum == pilot_checksum);

            const char* status = passed ? "PASS" : "FAIL";

            // 控制台输出与 CSV 文件均保存原始样本。
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
