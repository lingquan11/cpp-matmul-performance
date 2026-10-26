// GPT 指导模板，非用户原始源码。
// 用途：使用公式自动生成输入矩阵，并让循环边界跟随 n。
// 注意：这是参考实现，不声称与用户实际测试文件逐字一致。
#include <iostream>
using namespace std;

constexpr int MAX_N = 256;

int main() {
    int n = 3;

    static int A[MAX_N][MAX_N] = {};
    static int B[MAX_N][MAX_N] = {};
    static int C[MAX_N][MAX_N] = {};

    // 输入初始化：A[i][j] = i + j + 1，B[i][j] = 2*i + j + 1
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }

    // i-j-k 朴素矩阵乘法
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            C[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << C[i][j] << ' ';
        }
        cout << '\n';
    }
    return 0;
}
