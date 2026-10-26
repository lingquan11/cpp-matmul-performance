// BENCH_ID: ijk
// BENCH_LABEL: i-j-k 三重循环
// BENCH_LABEL_EN: i-j-k triple loop

// 我把 day02 的 i-j-k 拿过来，上界改成 n：
//   for i, for j:
//     C[i][j] = 0;
//     for k: C[i][j] += A[i][k] * B[k][j];
// 这个文件单独编译。基准不开 LTO，计时循环里的多次调用就会一次次执行。
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
