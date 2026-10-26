// BENCH_ID: ijk
// BENCH_LABEL: i-j-k 三重循环
// BENCH_LABEL_EN: i-j-k triple loop

// 与 src/day02_matrix.cpp 相同的 i-j-k 循环，只把固定上界 2 换成参数 n：
//   for i, for j:
//     C[i][j] = 0;
//     for k: C[i][j] += A[i][k] * B[k][j];
// 单独编译，且基准不开启 LTO，避免计时循环把多次调用折成一次。
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
