#include "ijk_kernel.h"

// 对应 src/day02_matrix.cpp 中的循环（原程序矩阵规模固定为 2）：
//   for i, for j:
//     C[i][j] = 0;
//     for k: C[i][j] += A[i][k] * B[k][j];
// 单独编译本文件，且基准程序不开启 LTO，避免调用被内联后只保留最后一次计算。
void matmul_ijk(const int* __restrict__ A, const int* __restrict__ B,
                int* __restrict__ C, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i * n + j] = 0;
            for (int k = 0; k < n; ++k) {
                C[i * n + j] += A[i * n + k] * B[k * n + j];
            }
        }
    }
}
