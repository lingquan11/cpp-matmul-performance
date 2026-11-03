// AI 维护：g++ -std=c++17 -O2 tests/packed-block-correctness.cpp -o packed-test
// 未实现返回 77（SKIP）；第 2 周再覆盖 n=70 和 b>n 等边界。
#define PACKED_BLOCK_TEST
#include "../templates/gpt/exp-packed-block.cpp"
#undef PACKED_BLOCK_TEST
#include <cstdint>
int main() {
    for(int n : {128,256}) for(int b : {32,64}) {
        initialize_inputs(n); alignas(64) static int packed[64*64]={};
        if(!pack_B_block(0,0,b,packed) || !matmul_blocked_packed(n,b)) { std::cerr << "SKIP：TODO(零醛) 未实现\n"; return 77; }
        for(int i=0;i<b;++i) for(int j=0;j<b;++j) {
            if(packed[i*b+j]!=B[i][j]) { std::cerr << "FAIL packing\n"; return 1; }
        }
        for(int i=0;i<n;++i) for(int j=0;j<n;++j) {
            std::int64_t ref=0; for(int k=0;k<n;++k) ref+=std::int64_t(A[i][k])*B[k][j];
            if(C[i][j]!=ref) { std::cerr << "FAIL\n"; return 1; }
        }
    }
    std::cout << "PASS\n";
}
