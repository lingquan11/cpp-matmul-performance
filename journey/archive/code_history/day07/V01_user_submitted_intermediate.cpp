#include <iostream>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <algorithm>  // 新增：使用 min() 处理分块边界

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

// 分块矩阵乘法：blocked ikj
// C 清零和矩阵乘法都计入计时。
NOINLINE void matmul_blocked(int n, int block_size) {

    // ① 清零有效区域：只清零一次，必须在所有分块循环之前。
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
        }
    }

    // ② 选择当前处理的行分块。
    for (int ii = 0; ii < n; ii += block_size) {
        const int i_end = min(ii + block_size, n);

        // ③ 选择当前处理的列分块。
        for (int jj = 0; jj < n; jj += block_size) {
            const int j_end = min(jj + block_size, n);

            // ④ 选择参与累加的 K 方向分块。
            for (int kk = 0; kk < n; kk += block_size) {
                const int k_end = min(kk + block_size, n);

                // ⑤ 分块内部仍然使用 i-k-j 顺序。
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

    // Day 7：需要比较的分块大小。
    const int block_sizes[] = { 16, 32, 64 };

    constexpr int BLOCK_SIZE_COUNT = 3;
    constexpr int ALGORITHM_COUNT = 5;

    /// 使用 Day 7 专属文件，不覆盖 Day 4、Day 5 数据。
    ofstream raw("matmul_benchmark_day7_blocking.csv");

    if (!raw) {
        cerr << "错误：无法创建 Day 7 CSV 文件" << endl;
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

        // =========================================
// 第三阶段：分块算法试测
// =========================================

        double pilot_blocked_ms[BLOCK_SIZE_COUNT] = {};
        int repeats_blocked[BLOCK_SIZE_COUNT] = {};

        for (int b = 0; b < BLOCK_SIZE_COUNT; ++b) {
            const int block_size = block_sizes[b];

            const auto blocked_pilot_start =
                chrono::steady_clock::now();

            matmul_blocked(n, block_size);

            const auto blocked_pilot_end =
                chrono::steady_clock::now();

            const chrono::duration<double, milli> blocked_elapsed =
                blocked_pilot_end - blocked_pilot_start;

            pilot_blocked_ms[b] = blocked_elapsed.count();

            // 校验操作全部在试测计时结束之后。
            const long long blocked_checksum = checksum_matrix(n);

            if (C[0][0] != expected ||
                blocked_checksum != reference_checksum ||
                !matches_reference(n)) {

                cerr << "错误：分块试测失败，n = "
                    << n << ", block_size = "
                    << block_size << endl;
                return 2;
            }

            // 根据每种分块大小的试测耗时确定正式测量次数。
            repeats_blocked[b] =
                choose_repeats(pilot_blocked_ms[b]);
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
        for (int b = 0; b < BLOCK_SIZE_COUNT; ++b) {
            cout << "blocked_" << block_sizes[b]
                << " 重复次数：" << repeats_blocked[b]
                << endl;
        }

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

        // 根据算法编号运行对应算法的一个正式样本。
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

        // 每种规模进行五次正式测量。
// 每个样本轮换算法执行顺序，降低固定先后顺序的影响。

        for (int sample = 1;
            sample <= FORMAL_SAMPLES;
            ++sample) {

            for (int offset = 0;
                offset < ALGORITHM_COUNT;
                ++offset) {

                const int algorithm_index =
                    (sample - 1 + offset) % ALGORITHM_COUNT;

                if (!run_algorithm_sample(
                    algorithm_index, sample)) {
                    return 2;
                }
            }
        }
        }
    }

    cout << "\nDay 7 分块实验完成。"
        << "原始数据已写入 "
        << "matmul_benchmark_day7_blocking.csv"
        << endl;

    return 0;
}
