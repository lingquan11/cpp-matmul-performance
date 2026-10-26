#ifndef BENCHMARKS_KERNEL_API_H
#define BENCHMARKS_KERNEL_API_H

// 基准驱动和 build/kernel_registry.cpp 共用的调度表。
// 增加内核时在 benchmarks/kernels/ 添加源文件。约定见 benchmarks/README.md。
//
// 每个 kernels/*.cpp 提供：
//   void matmul_<BENCH_ID>(const int* A, const int* B, int* C, int n);
// A、B、C 为行主序 n×n，三者互不重叠。
// 同一样本会连续调用多次，调用之间不会另做清零，函数内部要先把 C 的元素写成 0。

struct BenchImpl {
    const char* id;
    const char* label;
    void (*fn)(const int* A, const int* B, int* C, int n);
};

int bench_impl_count();
const BenchImpl* bench_impl_at(int index);

#endif
