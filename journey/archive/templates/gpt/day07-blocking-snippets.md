# Day 7 GPT 模板片段

> 本文件中的代码均为 GPT 提供的模板/集成片段，不是用户独立产出，也不是一个完整的单文件可编译程序。合并进现有项目时，需要保留原有全局数组、`NOINLINE` 定义、`checksum_matrix`、`matches_reference`、`expected_c00`、`choose_repeats` 和原有 `run_sample` 等函数。

## 1. 头文件

在文件开头增加：

```cpp
#include <algorithm>
```

## 2. 新增分块函数

放在 `matmul_ikj()` 之后、`checksum_matrix()` 之前：

```cpp
// 分块矩阵乘法：blocked ikj
// C 清零和矩阵乘法都计入计时。
NOINLINE void matmul_blocked(int n, int block_size) {
    // 清零有效区域，只执行一次。
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
        }
    }

    for (int ii = 0; ii < n; ii += block_size) {
        const int i_end = min(ii + block_size, n);

        for (int jj = 0; jj < n; jj += block_size) {
            const int j_end = min(jj + block_size, n);

            for (int kk = 0; kk < n; kk += block_size) {
                const int k_end = min(kk + block_size, n);

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
```

## 3. 边界测试片段

放在正式矩阵规模循环之前；它不写入正式性能 CSV，也不计入正式样本：

```cpp
{
    const int test_n = 70;
    const int test_block_size = 32;

    initialize_inputs(test_n);
    matmul_ijk(test_n);

    for (int i = 0; i < test_n; ++i) {
        for (int j = 0; j < test_n; ++j) {
            Reference[i][j] = C[i][j];
        }
    }

    const long long reference_sum = checksum_matrix(test_n);

    matmul_blocked(test_n, test_block_size);

    const bool passed =
        checksum_matrix(test_n) == reference_sum &&
        matches_reference(test_n);

    cout << "边界测试 n=70, block_size=32: "
        << (passed ? "PASS" : "FAIL") << endl;

    if (!passed) {
        cerr << "错误：分块边界测试失败。" << endl;
        return 2;
    }
}
```

## 4. 试测代码必须遵守的顺序

不要把分块试测放在 `Reference` 或 `reference_checksum` 建立之前。正确顺序：

1. 运行 `ijk`，检查 `C[0][0]`。
2. 将 `ijk` 的完整结果复制到 `Reference`。
3. 计算 `reference_checksum`。
4. 运行 `ikj`，并验证完整结果与参考矩阵一致。
5. 运行 `blocked_16`、`blocked_32`、`blocked_64`，与同一份 `Reference` 比较。
6. 为五种算法配置分别确定正式测量重复次数。
7. 开始正式测量。

`reference_checksum` 应在分块试测开始前声明并计算；不要重复声明同名变量。

## 5. 分块参数

在 `main()` 中：

```cpp
const int block_sizes[] = { 16, 32, 64 };
constexpr int BLOCK_SIZE_COUNT = 3;
constexpr int ALGORITHM_COUNT = 5;
```

## 6. 算法调度片段

在原有 `run_sample` lambda 之后，增加一个分发器；它复用原有计时和校验逻辑：

```cpp
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
```

正式测量循环：

```cpp
for (int sample = 1; sample <= FORMAL_SAMPLES; ++sample) {
    for (int offset = 0; offset < ALGORITHM_COUNT; ++offset) {
        const int algorithm_index =
            (sample - 1 + offset) % ALGORITHM_COUNT;

        if (!run_algorithm_sample(algorithm_index, sample)) {
            return 2;
        }
    }
}
```

## 7. 输出文件

使用 Day 7 独立文件，避免覆盖此前实验：

```cpp
ofstream raw("matmul_benchmark_day7_blocking.csv");
```
