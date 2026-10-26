// 来源：用户在对话中实际粘贴的代码
// 状态：用户提交版本；单次计时版本
#include <iostream>  // 引入输入输出功能
#include <chrono>
using namespace std;                      // 允许直接使用 cout

int main() {                              // 定义程序入口
    constexpr int MAX_N = 256;
    int n;

    cout << "请输入矩阵阶数 n（1-" << MAX_N << "）：";

    if (!(cin >> n) || n < 1 || n > MAX_N) {
        cerr << "错误：n 必须是 1 到 "
            << MAX_N << " 之间的整数。" << endl;
        return 1;
    }

    static int A[MAX_N][MAX_N] = {};
    static int B[MAX_N][MAX_N] = {};
    static int C[MAX_N][MAX_N] = {};

    // ① 初始化输入矩阵
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;/* TODO：根据 i、j 生成 A 的元素 */;
            B[i][j] = 2*i + j + 1;/* TODO：根据 i、j 生成 B 的元素 */;
        }
    }

    // ② 矩阵乘法：保留你已经写好的三重循环

    const auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;

            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    const auto end = std::chrono::steady_clock::now();

    const std::chrono::duration<double, std::milli> elapsed = end - start;

    cout << "计算耗时（包含 C 清零）: "
        << elapsed.count() << " ms" << endl;

    // ③ 输出结果：保留你已经写好的两重循环
    cout << "C[0][0] = " << C[0][0] << endl;

    return 0;                             // 返回 0，表示程序正常结束
}                                         // 结束 main 函数
