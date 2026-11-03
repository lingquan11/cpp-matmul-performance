# 正确性检查（不生成性能样本）

[`smoke.sh`](smoke.sh) 编译 `src/` 下所有 C++ 文件，检查 Day 1 的 `11`、Day 2 的 `11 10` / `19 30`，再运行 [`day07-correctness.cpp`](day07-correctness.cpp)。[GitHub Actions](../.github/workflows/compile.yml) 调用同一脚本。Day 4、Day 5、Day 7 的计时入口不运行；没有性能阈值，也不会写出 CSV。

## 最小运行

在仓库根目录，使用支持 C++17 的编译器。

Linux / macOS（需先有 g++）：

```sh
mkdir -p build
g++ -std=c++17 -O2 -Wall -Wextra -pedantic tests/day07-correctness.cpp -o build/day07-correctness
./build/day07-correctness 70 32
./build/day07-correctness
```

Windows：打开 Visual Studio 的 **x64 Native Tools Command Prompt**，进入仓库根目录：

```bat
if not exist build mkdir build
cl /nologo /std:c++17 /EHsc /O2 /utf-8 /W4 tests\day07-correctness.cpp /Fe:build\day07-correctness.exe /Fo:build\day07-correctness.obj
build\day07-correctness.exe 70 32
build\day07-correctness.exe
```

第一条运行指定 `n=70`、`block_size=32`，检查 12 个算法/输入组合；无参数运行完整 392 个组合。成功输出以 `PASS:` 开头，退出码为 0。参数必须恰好为两个十进制正整数：`1 <= n <= 256`，`1 <= block_size <= 257`。例如 `70 0`、`257 32`、`70 1x` 会报告用法并退出 2；正确性失败退出 1。允许块大于矩阵，便于检验边界截断，但本测试工具不接受任意大的块。

想运行与 CI 一致的全部检查，在有 Bash 和 g++ 的环境执行：

```sh
bash tests/smoke.sh
```

编译器不在 PATH 时，Windows 请使用上面的开发者命令提示符；无需创建 Visual Studio 工程。若找不到源码，检查当前目录是否为仓库根目录。这里不需要 Python 或额外 C++ 测试框架。

## 改 matrix 规模与分块大小（只读源码，不改仓库）

正式性能实验的规模写在 `src/` 里，**没有命令行参数**。下列位置仅供你在本机改常量后重跑；新 CSV 不覆盖已有归档；如决定公开新实验，应独立建目录并记录环境和来源。

| 程序 | 改什么 | 源码位置（行号随提交可能略变） | 运行产物 |
| --- | --- | --- | --- |
| Day 4 单次计时 | 矩阵阶数 `n` | 运行时在 stdin 输入 1–256 | 仅终端输出 |
| Day 5 循环顺序 | 测试阶数列表 | `src/day05-ijk-ikj-benchmark.cpp` 中 `sizes[]`（默认 64、128、256）与 `MAX_N` | 当前目录 `matmul_benchmark_day5_loop_order.csv` |
| Day 7 分块 | 阶数与块大小 | 同文件 `sizes[]`、`block_sizes[]`（默认 16/32/64）；边界测试常量 `test_n` / `test_block_size` | 当前目录 `matmul_benchmark_day7_blocking.csv` |

Day 7 使用固定容量 `int[256][256]`，逻辑 `n < 256` 时行跨度仍是 256 个 `int`（见 [`src/README.md`](../src/README.md)）。改 `MAX_N` 或输入公式前须重新检查溢出与正确性测试。

Windows（Release/x64，与历史 CSV 同口径时尽量对齐 Visual Studio 2026 配置）示例——**会长时间运行并写 CSV**：

```bat
cl /nologo /std:c++17 /O2 /EHsc /utf-8 src\day05-ijk-ikj-benchmark.cpp /Fe:build\day05.exe
build\day05.exe
```

Linux 上可同样编译运行，但 **VM 或 g++ 的毫秒不能写入正式报告**；只用于理解输出格式。归档数字仍以 Windows 程序导出 CSV 为准。

## 检查了什么

- 尺寸：`1, 2, 15, 16, 17, 31, 32, 33, 63, 64, 65, 70, 255, 256`，覆盖分块边缘、非整除及固定容量上限。
- 配置：`ijk`、`ikj`、分块 `1 / 16 / 32 / 64 / (n+1)`，共 7 种。
- 输入：原初始化公式、右侧零矩阵、右侧单位矩阵、确定性的正负小整数，4 种。
- 每一项与独立 `int64_t` 点积参考逐元素精确比较；不把被测 `ijk` 的结果当成其他算法的真值。
- 每次调用先把整个 C 填入非零哨兵，检查有效区域确实清零并计算，`n × n` 外的 C 保持不变。这里是固定容量数组内的哨兵检查，不能代替内存检测工具。
- 冒烟脚本另检查 8 组非法参数均退出 2，避免无效尺寸或零块进入计算。

测试直接包含现有 Day 7 源文件，通过宏把其 `main` 改名；测试自己的入口只调用计算函数。这样保留学习源码和基线，不复制待测算法，也不运行计时、预热或 CSV 输出。取舍是测试依赖该文件的全局数组和函数名，接口变化时需要同步调整适配器。测试适配代码由 AI 维护，不代表学习者新增了核心算法实现。

## 适用限制

现有实现是单线程、固定行跨度 256 的 `int` 方阵。这里不测试矩形、浮点数、任意整数范围或并行实现。原初始化在 `n <= 256` 时，每个累加结果的保守上界为 `256 × 511 × 766 = 100205056`，小于 32 位有符号整数上限；正负输入的绝对和至多 `256 × 5 × 6 = 7680`。因此本套输入使用精确整数比较，不涉及浮点误差容限。修改数据类型或扩大范围后，须重新检查溢出和误差标准。

正确性通过只能说明这些输入与边界组合通过。它不是用户历史 75 条 CSV 的逐行复现，也不能证明分块性能更好或缓存未命中更少。历史 Day 7 程序仍是 GPT 整合版，来源说明保持不变。

## 行跨度与 SIMD 脚手架检查（未接入 smoke）

[`layout-lda-correctness.cpp`](layout-lda-correctness.cpp) 用 `#define main` 引入 [`templates/gpt/layout-lda.cpp`](../templates/gpt/layout-lda.cpp)。两个 LDA kernel 还是 `TODO(零醛)` 时，它打印 `SKIP`，退出码 0。这不是 `PASS`。n=70 以及 `lda>n` 的逐元素比对要等 kernel 实现之后才会执行。

[`simd-factor-check.cpp`](simd-factor-check.cpp) 同样只在 kernel 未实现时打印 `SKIP`。它检查 build 标记和「n=1024 不能沿用 day07 的 int 公式」，不跑计时。

两个检查都不要接进 [`smoke.sh`](smoke.sh)，待零醛实现后接入。`smoke.sh` 会编译 `src/` 下全部 `.cpp`，但不会编译 `templates/`。

格式测试，非实测：

```sh
python3 tests/test_scan_asm_keywords.py
python3 tests/test_summarize_layout_lda.py
```

夹具在 [`fixtures/asm-keyword-fixture/`](fixtures/asm-keyword-fixture/README.md) 和 [`fixtures/layout-lda-fixture/`](fixtures/layout-lda-fixture/README.md)。里面的数字和指令计数不能写进正式 CSV 或报告。

在仓库根目录单独编译并跑脚手架检查：

```sh
bash scripts/build-lda-simd.sh
```

核心未实现时，`./build/layout-lda --algo ijk --n 64 --lda n --runs 1 --out build/layout-lda.csv` 应打印「未实现」并以非零码退出，且不创建 CSV。SIMD 那份同样。实现并自测通过后，把对应文件里的 `kLayoutLdaImplemented` 或 `kSimdKernelImplemented` 改为 `true`，确认检查输出以 `PASS:` 开头，再决定是否写入 `smoke.sh`。

## 1002-I4 脚手架测试（未接入 smoke）

下面两项不在 [`smoke.sh`](smoke.sh) 里。四种新循环顺序和三个访存计数还是 `TODO(零醛)`。待零醛实现后接入。

```sh
g++ -std=c++17 -O2 -Wall -Wextra -pedantic -o build/loop-orders-correctness tests/loop-orders-correctness.cpp
build/loop-orders-correctness
python3 tests/test_dataflow_ledger.py
python3 scripts/dataflow-ledger.py --M 64 --K 64 --N 64 --R 8 --C 8 --dataflow OS
```

循环顺序测试在 `n ∈ {1,2,70,255,256}` 上检查。`ijk` 和 `ikj` 与独立 int64 参考比较；`jik`、`kij`、`jki`、`kji` 未实现时打印 `SKIP`。未实现的桩若改写了 C，测试失败。账本脚本未实现时退出码非零，测试把数值对照记为 `SKIP`。

Claude 给出的参考值，待零醛手推核对；WS/IS 的值取决于计数口径。`(64,64,64)` 在 8×8 上是 OS 69632、WS 98304、IS 98304；在 16×4 上是 OS 86016、WS 98304、IS 147456。提示词只给了这些总数，没有 A/B/C 分项。计数口径（待零醛确认）：C 的部分和第一趟是否计读？只计片外/缓冲访问，还是也计 PE 间传递？这些数不是本仓库的实测，也不是程序当前的输出。
