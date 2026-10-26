#include <iostream>
using namespace std;

int main() {
    int A[2][2] = { { 2, 1 },{ 3, 4 } };
    int B[2][2] = { { 5, 2 },{ 1, 6 } };
    int c00 = 0;

    for (int k = 0; k < 2; ++k) {
        c00 += A[0][k] * B[k][0];// 将对应元素的乘积累加到 c00
    }

    cout << c00 << endl;
    return 0;
}