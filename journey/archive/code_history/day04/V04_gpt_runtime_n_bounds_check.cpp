// GPT 指导模板，非用户原始源码。
// 用途：运行时输入矩阵规模，并检查 1 <= n <= MAX_N。
// 注意：这是参考实现，不声称与用户实际测试文件逐字一致。
#include <iostream>
using namespace std;

constexpr int MAX_N = 256;

int main() {
    int n;
    cout << "请输入矩阵阶数 n（1-" << MAX_N << "）：";

    if (!(cin >> n) || n < 1 || n > MAX_N) {
        cerr << "错误：n 必须是 1 到 " << MAX_N
             << " 之间的整数。" << endl;
        return 1;
    }

    static int A[MAX_N][MAX_N] = {};
    static int B[MAX_N][MAX_N] = {};
    static int C[MAX_N][MAX_N] = {};

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            A[i][j] = i + j + 1;
            B[i][j] = 2 * i + j + 1;
        }
    }

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
