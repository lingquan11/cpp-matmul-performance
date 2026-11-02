// AI 维护的测试适配器；g++ -std=c++17 -O2 tests/scale-sweep-correctness.cpp -o scale-test
// 未实现时返回 77 (SKIP)，不接入 smoke；不生成性能样本。
#define main scale_sweep_main
#include "../templates/gpt/scale-sweep.cpp"
#undef main
#include <limits>
int main() {
    for (int n : {1,70,255,256,384}) for (int extra : {0,16}) {
        int ld=n+extra; size_t count=static_cast<size_t>(n)*ld;
        vector<int> a(count),b(count),c(count);
        if(!fill_inputs_bounded(a,b,n,ld)) { cerr << "SKIP：TODO(零醛) 未实现\n"; return 77; }
        for(int block : {0,64,128}) {
            fill(c.begin(),c.end(),0);
            bool ok=block?matmul_blocked_ld(a,b,c,n,ld,block):matmul_ikj_ld(a,b,c,n,ld);
            if(!ok) { cerr << "SKIP：TODO(零醛) 未实现\n"; return 77; }
            for(int i=0;i<n;++i) for(int j=0;j<n;++j) {
                int64_t ref=0; for(int k=0;k<n;++k) ref+=int64_t(a[i*ld+k])*b[k*ld+j];
                if(ref!=c[i*ld+j]) { cerr << "FAIL\n"; return 1; }
            }
        }
    }
    cout << "PASS\n"; return 0;
}
