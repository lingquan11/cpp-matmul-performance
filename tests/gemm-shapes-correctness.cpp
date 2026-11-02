// AI 维护：g++ -std=c++17 -O2 tests/gemm-shapes-correctness.cpp -o gemm-test
// 核心未实现返回 77（SKIP），不是 PASS；暂不接入 smoke。
#define GEMM_SHAPES_TEST
#include "../templates/gpt/exp-gemm-shapes.cpp"
int main() {
    for(Shape s : {Shape{3,5,7},Shape{1,16,1}}) {
        std::vector<int>a(size_t(s.m)*s.k),b(size_t(s.k)*s.n),c(size_t(s.m)*s.n,99);
        for(size_t i=0;i<a.size();++i) a[i]=int(i%7)-3;
        for(size_t i=0;i<b.size();++i) b[i]=int(i%5)-2;
        if(!ikj_mkn(s.m,s.k,s.n,a,b,c)) { std::cerr<<"SKIP：TODO(零醛) 未实现\n"; return 77; }
        for(int i=0;i<s.m;++i) for(int j=0;j<s.n;++j) {
            std::int64_t ref=0; for(int x=0;x<s.k;++x) ref+=std::int64_t(a[size_t(i)*s.k+x])*b[size_t(x)*s.n+j];
            if(c[size_t(i)*s.n+j]!=ref) { std::cerr<<"FAIL\n"; return 1; }
        }
    }
    std::cout<<"PASS\n";
}
