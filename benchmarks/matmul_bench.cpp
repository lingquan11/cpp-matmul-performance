#include "ijk_kernel.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

struct Options {
    bool check_only = false;
    std::vector<int> sizes;
    int runs = 5;
    double min_ms = 100.0;
    std::string csv_path;
};

struct Row {
    int n = 0;
    int sample = 0;
    int reps = 0;
    long long elapsed_ns = 0;
    double gflops = 0.0;
    long long checksum = 0;
};

void usage() {
    std::cerr
        << "用法:\n"
        << "  matmul_bench --check\n"
        << "  matmul_bench --sizes 64,128,256 --runs 5 --min-ms 100 --csv out.csv\n";
}

bool parse_sizes(const std::string& text, std::vector<int>* sizes) {
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (item.empty()) {
            return false;
        }
        char* end = nullptr;
        long value = std::strtol(item.c_str(), &end, 10);
        if (end == item.c_str() || *end != '\0' || value <= 0 || value > 4096) {
            return false;
        }
        sizes->push_back(static_cast<int>(value));
    }
    return !sizes->empty();
}

bool parse_args(int argc, char** argv, Options* opt) {
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto need = [&](const char* name) -> const char* {
            if (i + 1 >= argc) {
                std::cerr << "缺少参数值: " << name << "\n";
                std::exit(2);
            }
            return argv[++i];
        };
        if (arg == "--check") {
            opt->check_only = true;
        } else if (arg == "--sizes") {
            if (!parse_sizes(need("--sizes"), &opt->sizes)) {
                std::cerr << "无法解析 --sizes\n";
                return false;
            }
        } else if (arg == "--runs") {
            opt->runs = std::atoi(need("--runs"));
        } else if (arg == "--min-ms") {
            opt->min_ms = std::atof(need("--min-ms"));
        } else if (arg == "--csv") {
            opt->csv_path = need("--csv");
        } else if (arg == "--help" || arg == "-h") {
            usage();
            std::exit(0);
        } else {
            std::cerr << "未知参数: " << arg << "\n";
            return false;
        }
    }
    if (!opt->check_only && (opt->sizes.empty() || opt->csv_path.empty())) {
        std::cerr << "正式测量需要 --sizes 和 --csv\n";
        return false;
    }
    if (opt->runs <= 0 || opt->min_ms <= 0.0) {
        std::cerr << "--runs 与 --min-ms 必须为正\n";
        return false;
    }
    return true;
}

int value_a(int i, int j) { return (i * 13 + j * 7 + 3) % 5; }

int value_b(int i, int j) { return (i * 11 + j * 5 + 1) % 5; }

void fill_inputs(std::vector<int>* A, std::vector<int>* B, int n) {
    A->assign(static_cast<size_t>(n) * n, 0);
    B->assign(static_cast<size_t>(n) * n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            (*A)[i * n + j] = value_a(i, j);
            (*B)[i * n + j] = value_b(i, j);
        }
    }
}

long long reference_checksum(const std::vector<int>& A, const std::vector<int>& B, int n) {
    long long sum = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            long long acc = 0;
            for (int k = 0; k < n; ++k) {
                acc += static_cast<long long>(A[i * n + k]) * B[k * n + j];
            }
            sum += acc;
        }
    }
    return sum;
}

long long matrix_checksum(const std::vector<int>& C) {
    long long sum = 0;
    for (int value : C) {
        sum += value;
    }
    return sum;
}

int fail(const std::string& message) {
    std::cerr << "正确性检查失败: " << message << "\n";
    return 1;
}

int run_checks() {
    {
        const int n = 2;
        const int A[4] = {2, 1, 3, 4};
        const int B[4] = {5, 2, 1, 6};
        int C[4] = {0, 0, 0, 0};
        matmul_ijk(A, B, C, n);
        const int expect[4] = {11, 10, 19, 30};
        for (int i = 0; i < 4; ++i) {
            if (C[i] != expect[i]) {
                return fail("Day 2 原始输入结果与手算不一致");
            }
        }
    }
    {
        const int n = 2;
        const int A[4] = {2, 1, 3, 4};
        const int B[4] = {1, 0, 0, 1};
        int C[4] = {0, 0, 0, 0};
        matmul_ijk(A, B, C, n);
        for (int i = 0; i < 4; ++i) {
            if (C[i] != A[i]) {
                return fail("右乘单位矩阵后结果不是 A");
            }
        }
    }
    {
        const int n = 2;
        const int A[4] = {2, 1, 3, 4};
        const int B[4] = {0, 0, 0, 0};
        int C[4] = {7, 7, 7, 7};
        matmul_ijk(A, B, C, n);
        for (int i = 0; i < 4; ++i) {
            if (C[i] != 0) {
                return fail("右乘零矩阵后结果不是零矩阵");
            }
        }
    }
    {
        const int n = 4;
        std::vector<int> A(16, 1);
        std::vector<int> B(16, 1);
        std::vector<int> C(16, 9);
        matmul_ijk(A.data(), B.data(), C.data(), n);
        for (int value : C) {
            if (value != n) {
                return fail("全 1 矩阵的结果不是 n");
            }
        }
    }
    {
        const int n = 3;
        std::vector<int> A;
        std::vector<int> B;
        fill_inputs(&A, &B, n);
        std::vector<int> C(static_cast<size_t>(n) * n, -1);
        matmul_ijk(A.data(), B.data(), C.data(), n);
        if (matrix_checksum(C) != reference_checksum(A, B, n)) {
            return fail("n=3 校验和与逐项乘加不一致");
        }
    }
    std::cout << "正确性检查通过\n";
    return 0;
}

int calibrate_reps(const int* A, const int* B, int* C, int n, double min_ms) {
    int batch = 1;
    long long elapsed_ns = 0;
    const long long target_cal_ns = 20LL * 1000LL * 1000LL;
    while (true) {
        const auto t0 = std::chrono::steady_clock::now();
        for (int r = 0; r < batch; ++r) {
            matmul_ijk(A, B, C, n);
        }
        const auto t1 = std::chrono::steady_clock::now();
        elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
        if (elapsed_ns >= target_cal_ns || batch >= 100000) {
            break;
        }
        batch = batch > 50000 ? 100000 : batch * 2;
    }
    if (elapsed_ns <= 0) {
        std::cerr << "校准计时为 0，n=" << n << "\n";
        std::exit(1);
    }
    const double per_ns = static_cast<double>(elapsed_ns) / static_cast<double>(batch);
    const double target_ns = min_ms * 1e6;
    long long reps = static_cast<long long>(std::ceil(target_ns / per_ns));
    if (reps < 1) {
        reps = 1;
    }
    if (reps > 100000) {
        reps = 100000;
    }
    return static_cast<int>(reps);
}

double gflops_of(int n, long long elapsed_ns, int reps) {
    const double time_s =
        static_cast<double>(elapsed_ns) / static_cast<double>(reps) / 1e9;
    const double flops =
        2.0 * static_cast<double>(n) * static_cast<double>(n) * static_cast<double>(n);
    return flops / time_s / 1e9;
}

int run_bench(const Options& opt) {
    std::vector<Row> rows;
    for (int n : opt.sizes) {
        std::vector<int> A;
        std::vector<int> B;
        fill_inputs(&A, &B, n);
        std::vector<int> C(static_cast<size_t>(n) * n, 0);
        const long long expect = reference_checksum(A, B, n);

        matmul_ijk(A.data(), B.data(), C.data(), n);
        if (matrix_checksum(C) != expect) {
            std::cerr << "测量前校验失败, n=" << n << "\n";
            return 1;
        }

        const int reps = calibrate_reps(A.data(), B.data(), C.data(), n, opt.min_ms);
        if (matrix_checksum(C) != expect) {
            std::cerr << "校准后校验失败, n=" << n << "\n";
            return 1;
        }
        std::cerr << "n=" << n << " 样本内重复=" << reps << "\n";

        for (int sample = 1; sample <= opt.runs; ++sample) {
            const auto t0 = std::chrono::steady_clock::now();
            for (int r = 0; r < reps; ++r) {
                matmul_ijk(A.data(), B.data(), C.data(), n);
            }
            const auto t1 = std::chrono::steady_clock::now();
            const long long elapsed_ns =
                std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
            if (elapsed_ns <= 0) {
                std::cerr << "正式样本计时为 0, n=" << n << "\n";
                return 1;
            }
            const long long checksum = matrix_checksum(C);
            if (checksum != expect) {
                std::cerr << "正式样本校验失败, n=" << n << " sample=" << sample << "\n";
                return 1;
            }
            Row row;
            row.n = n;
            row.sample = sample;
            row.reps = reps;
            row.elapsed_ns = elapsed_ns;
            row.gflops = gflops_of(n, elapsed_ns, reps);
            row.checksum = checksum;
            rows.push_back(row);
            std::cerr << "  sample " << sample << "/" << opt.runs << " elapsed_ns=" << elapsed_ns
                      << " gflops=" << row.gflops << "\n";
        }
    }

    std::ofstream out(opt.csv_path);
    if (!out) {
        std::cerr << "无法写入 " << opt.csv_path << "\n";
        return 1;
    }
    out << "impl,n,sample,reps,elapsed_ns,gflops,checksum\n";
    out.setf(std::ios::fmtflags(0), std::ios::floatfield);
    out.precision(17);
    for (const Row& row : rows) {
        out << "ijk," << row.n << "," << row.sample << "," << row.reps << "," << row.elapsed_ns
            << "," << row.gflops << "," << row.checksum << "\n";
    }
    std::cout << "已写入 " << opt.csv_path << "\n";
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    Options opt;
    if (!parse_args(argc, argv, &opt)) {
        usage();
        return 2;
    }
    const int check_status = run_checks();
    if (check_status != 0 || opt.check_only) {
        return check_status;
    }
    return run_bench(opt);
}
