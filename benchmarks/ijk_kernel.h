#ifndef BENCHMARKS_IJK_KERNEL_H
#define BENCHMARKS_IJK_KERNEL_H

// 与 src/day02_matrix.cpp 相同的 i-j-k 三重循环，仅把固定上界 2 换成参数 n。
// A、B、C 为行主序 n×n，互不重叠（对应原程序里三个独立数组）。
// 这里不实现循环重排或分块；仓库里没有这些版本。
void matmul_ijk(const int* __restrict__ A, const int* __restrict__ B,
                int* __restrict__ C, int n);

#endif
