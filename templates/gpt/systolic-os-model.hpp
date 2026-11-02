// 软件输出驻留（output-stationary）脉动阵列周期模型：声明。
// 脚手架由 AI 维护；标 TODO(零醛) 的函数由零醛本人实现，不是 AI 代写的学生实现。
//
// 这是什么：参数、网格、注入队列、计数器和解析式的声明。没有拍推进、没有输入错位、没有 PE 乘加。
// 谁写的：AI 脚手架。零醛实现三个核心函数。
// 怎么编译：见 systolic-os-model.cpp 文件头，或 scripts/build-systolic-os-model.sh。
// 限制：软件模型，不是硬件，也不是性能基准。运行耗时不能写入 experiments/results。
//       日程序号未定，所以本文件不在 src/。tests/smoke.sh 会编译 src/ 下全部 cpp，实现前不要拷进去。
//
// 拍次约定（与 explorer 仓库的 2×2 练习对齐的是拍次，不是同一组矩阵）：
// 从 0 开始计拍。PE(i,j) 在第 t = i+j+k 拍收到第 k 对数据（i、j、k 都从 0 起，i/j 是 tile 内坐标）。
// 本仓库 Day 2 数据是 A=[[2,1],[3,4]]、B=[[5,2],[1,6]]、C=[[11,10],[19,30]]。
// explorer 练习 A 是 [[1,2],[3,4]]×[[5,6],[7,8]]=[[19,22],[43,50]]。数据不合并。
//
// 解析式（待零醛手算核对，不是模拟器跑出来的）：
// expected_cycles(n,P) = (ceil(n/P))^2 * (n + 2P - 2)
// 单个 tile、归约长度 K 时，拍数是 K+2P-2。本模型按输出 tile 划分，K 取整段 n，空间边缘补零。

#ifndef SYSTOLIC_OS_MODEL_HPP
#define SYSTOLIC_OS_MODEL_HPP

#include <deque>
#include <vector>

// TODO(零醛): 三个核心函数实现并核对之后，把下一行改成 true。
// false 时：测试输出 SKIP，不输出 PASS；可执行文件进入核心函数后打印「未实现」并以非零码退出。
inline constexpr bool kSystolicCoreImplemented = false;

// 脚手架容量，和本仓库现有方阵上限同量级。不是测出来的性能边界。
inline constexpr int kMaxN = 256;
inline constexpr int kMaxP = 64;

inline constexpr char kCsvHeader[] =
    "n,P,tiles,cycles,macs,util,buf_reads_A,buf_reads_B";

struct OsPe {
    int acc = 0;             // 输出驻留的部分和。step_one_cycle 负责更新。
    int a_in = 0;            // 本拍流入的 a。step_one_cycle 负责写入。
    int b_in = 0;            // 本拍流入的 b。step_one_cycle 负责写入。
    long long buf_reads_a = 0;
    long long buf_reads_b = 0;
};

struct SystolicModel {
    int n = 0;
    int P = 0;
    // 已完成的拍数。prepare 之后是 0。每次 step_one_cycle 正常返回后加 1。
    // 刚完成的拍次下标（从 0 起）是 cycles_done - 1。跨 tile 累加，不要在每个 tile 清零。
    long long cycles_done = 0;
    long long macs = 0;
    long long buf_reads_A = 0;
    long long buf_reads_B = 0;
    bool trace = false;
    std::vector<std::vector<OsPe>> pe;          // pe[i][j]，tile 内 P×P
    std::vector<std::deque<int>> left_a;        // 左侧 A 注入队列，每行一条。inject 负责填。
    std::vector<std::deque<int>> top_b;         // 上侧 B 注入队列，每列一条。inject 负责填。
    std::vector<std::vector<int>> A;
    std::vector<std::vector<int>> B;
    std::vector<std::vector<long long>> C;      // run_tile 把本 tile 的结果写入这里。
};

// 正整数的向上取整除法。n 或 P 不是正数时返回 -1。
long long ceil_div_positive(int n, int P);

// tile 个数 = (ceil(n/P))^2。待零醛手算核对。
long long expected_tiles(int n, int P);

// 总拍数 = expected_tiles(n,P) * (n + 2P - 2)。待零醛手算核对。
long long expected_cycles(int n, int P);

// 分配网格和矩阵，填入输入，计数器清零。不做 skew，不推拍，不做乘加。
// n==2 用 Day 2 固定矩阵；其它 n 用下面的确定性公式（与 Day 5 模板同一式，方便以后对账）：
// A[i][j] = i+j+1，B[i][j] = 2*i+j+1。这是脚手架约定，不是新的测量。
bool prepare_model(SystolicModel& model, int n, int P, const char*& error);

void matmul_ikj_reference(const std::vector<std::vector<int>>& A,
                          const std::vector<std::vector<int>>& B,
                          std::vector<std::vector<long long>>& C);

// 逐元素精确比较 model.C 和 ikj。oracle 只用于对账，不是被测的脉动核心。
bool same_as_ikj(const SystolicModel& model, const char*& why);

// 按当前计数器写一行 CSV。拒绝覆盖已有文件。不在核心未实现时被 main 调用。
bool write_model_csv(const char* path, const SystolicModel& model, const char*& error);

// 打印当前 PE 的 acc 和本拍流入的 a/b。只读状态，不推拍。
void print_cycle_snapshot(const SystolicModel& model);

// TODO(零醛): 把当前输出 tile 的 A/B 按 skew 放进 left_a / top_b。
// tile 内坐标 i、j 从 0 起；超出 n 的空间边缘补 0。不要在这里做乘加。
void inject_skewed_inputs(SystolicModel& model, int tile_i, int tile_j);

// TODO(零醛): 推进一拍：数据移动、PE 乘加、cycles_done / macs / buf_reads 加一拍的量。
// model.trace 为 true 时调用 print_cycle_snapshot。不要在这里循环完整个 tile。
void step_one_cycle(SystolicModel& model);

// TODO(零醛): 跑完一个输出 tile，内部再调用上面两个函数。
// 本 tile 拍数按 n+2P-2（K 取整段 n）。空间不够 P 的一边补零。
// 把落在 n×n 里的结果写入 model.C。cycles_done 跨 tile 累加。
void run_tile(SystolicModel& model, int tile_i, int tile_j);

// 按输出 tile 逐个调用 run_tile。这里没有数据移动，也没有乘加。
void run_all_tiles(SystolicModel& model);

#endif
