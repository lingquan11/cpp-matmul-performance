// GPT 指导模板，非用户原始源码。
// 用途：展示固定数组容量 MAX_N 与当前有效规模 n 分离。
// 注意：该参考版本根据教学中的代码片段整理，不代表用户电脑上实际运行的完整源码。
#include <iostream>
using namespace std;

constexpr int MAX_N = 256;

int main() {
    int n = 3;

    static int A[MAX_N][MAX_N] = {
        {2, 1, 1},
        {3, 4, 2},
        {1, 0, 3}
    };
    static int B[MAX_N][MAX_N] = {
        {5, 2, 1},
        {1, 6, 0},
        {2, 1, 4}
    };
    static int C[MAX_N][MAX_N] = {};

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
