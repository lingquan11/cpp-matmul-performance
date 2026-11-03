// AI 维护脚手架；TODO(零醛) 由零醛实现，不是 AI 代写的学生实现。
// g++ -std=c++17 -O2 templates/gpt/exp-gemm-shapes.cpp -o gemm-shapes
// --help 查看参数；公式是待确认的 Day 7 推广，不能直接当作已认可的实验输入。
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>
struct Shape { int m,k,n; };
// TODO(零醛)：实现矩形 ikj，C 清零须包含在内。
bool ikj_mkn(int, int, int, const std::vector<int>&, const std::vector<int>&, std::vector<int>&) { return false; }
// 原公式非负；最大项的 K 倍给出保守上界。逐步除法避免检查自身溢出。
bool safe_bound(int m,int k,int n) {
    if(m<=0 || k<=0 || n<=0 || m>100000 || k>100000 || n>100000) return false;
    std::int64_t a=std::int64_t(m)+k-1, b=2LL*k+n-2, limit=std::numeric_limits<int>::max();
    return a<=limit && b<=limit && a<=limit/k && b<=limit/(a*k);
}
#ifndef GEMM_SHAPES_TEST
int main(int argc,char** argv) {
    int m=0,k=0,n=0,runs=0; std::string shape,out;
    try {
        for(int i=1;i<argc;++i) {
            std::string key=argv[i]; if(key=="--help") { std::cout<<"--M M --K K --N N OR --shape {square,tall,wide,deep,shortk} --runs N --out CSV\n"; return 0; }
            if(i+1>=argc) throw std::invalid_argument("missing value");
            std::string v=argv[++i]; size_t pos=0;
            if(key=="--shape") shape=v; else if(key=="--out") out=v;
            else if(key=="--M" || key=="--K" || key=="--N" || key=="--runs") { int x=std::stoi(v,&pos); if(pos!=v.size() || x<=0) throw std::invalid_argument("positive integer required"); if(key=="--M") m=x; else if(key=="--K") k=x; else if(key=="--N") n=x; else runs=x; }
            else throw std::invalid_argument("unknown option");
        }
        if(!shape.empty()) {
            if(m || k || n) throw std::invalid_argument("choose shape or dimensions");
            Shape s;
            if(shape=="square") s={256,256,256}; else if(shape=="tall") s={2048,64,128}; else if(shape=="wide") s={32,512,1024}; else if(shape=="deep") s={256,1024,64}; else if(shape=="shortk") s={1024,16,1024}; else throw std::invalid_argument("unknown shape");
            m=s.m;k=s.k;n=s.n;
        }
        if(out.empty() || runs<=0 || !safe_bound(m,k,n)) throw std::invalid_argument("invalid dimensions or int32 overflow bound");
        std::vector<int> a(size_t(m)*k),b(size_t(k)*n),c(size_t(m)*n);
        for(int i=0;i<m;++i) for(int x=0;x<k;++x) a[size_t(i)*k+x]=i+x+1;
        for(int x=0;x<k;++x) for(int j=0;j<n;++j) b[size_t(x)*n+j]=2*x+j+1;
        // Check implementation before opening output; do not emit fabricated data.
        if(!ikj_mkn(m,k,n,a,b,c)) { std::cerr<<"未实现: ikj_mkn TODO(零醛)\n"; return 2; }
        std::ofstream csv(out); if(!csv) throw std::runtime_error("cannot open output");
        csv<<"M,K,N,sample,ms,gmac_per_s,checksum\n";
        for(int r=1;r<=runs;++r) {
            auto t0=std::chrono::steady_clock::now(); bool ok=ikj_mkn(m,k,n,a,b,c); auto t1=std::chrono::steady_clock::now();
            if(!ok) { std::cerr<<"未实现: ikj_mkn\n"; return 2; }
            double ms=std::chrono::duration<double,std::milli>(t1-t0).count(); if(ms<=0) throw std::runtime_error("invalid elapsed time");
            std::int64_t sum=0; for(int value:c) sum+=value;
            csv<<m<<','<<k<<','<<n<<','<<r<<','<<ms<<','<<(double(m)*k*n/(ms*1e6))<<','<<sum<<'\n';
        }
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 2; }
}
#endif
