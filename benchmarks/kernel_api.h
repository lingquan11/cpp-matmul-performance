#ifndef BENCHMARKS_KERNEL_API_H
#define BENCHMARKS_KERNEL_API_H

// 这张表给我的计时程序和 build/kernel_registry.cpp 一起用。
// 新的循环我写在 benchmarks/kernels/ 里，约定在 benchmarks/README.md。
//
// 每个 kernels/*.cpp 提供：
//   void matmul_<BENCH_ID>(const int* A, const int* B, int* C, int n);
// A、B、C 是行主序的 n×n，三块内存不重叠。
// 一次样本里我会连着调用很多次，中间不再清 C，所以函数自己先把 C 写成 0。

struct BenchImpl {
    const char* id;
    const char* label;
    void (*fn)(const int* A, const int* B, int* C, int n);
};

int bench_impl_count();
const BenchImpl* bench_impl_at(int index);

#endif
