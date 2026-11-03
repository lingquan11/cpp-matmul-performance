// AI 维护的规模扫描脚手架；TODO(零醛) 由零醛本人实现，不是学生实现。
// 构建：g++ -std=c++17 -O2 templates/gpt/scale-sweep.cpp -o scale-sweep
// 使用：scale-sweep --help。输入与 kernel 未完成时不产生样本。
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;
// 原输入在大规模可能溢出 int32；有界输入及其累加上界由零醛选定。
bool fill_inputs_bounded(vector<int>&, vector<int>&, int, int) { // TODO(零醛)
    return false;
}
bool matmul_ikj_ld(const vector<int>&, const vector<int>&, vector<int>&, int, int) { // TODO(零醛)
    return false;
}
bool matmul_blocked_ld(const vector<int>&, const vector<int>&, vector<int>&, int, int, int) { // TODO(零醛)
    return false;
}
int main(int argc, char** argv) {
    int n=0, runs=1, timeout=15, requested_block=0; string algo, out;
    try {
        for(int i=1;i<argc;++i) {
            string key=argv[i];
            if(key=="--help") { cout << "--n N --algo {ikj,blocked,blocked_64,blocked_128,blocked_packed_64,ikj_lda16} --block {16,32,64,128,256} --runs N --out CSV --timeout-sec N (default 15)\n"; return 0; }
            if(i+1==argc) throw invalid_argument("missing value");
            string v=argv[++i]; size_t pos=0;
            if(key=="--algo") algo=v;
            else if(key=="--out") out=v;
            else if(key=="--n" || key=="--runs" || key=="--timeout-sec" || key=="--block") {
                int x=stoi(v,&pos); if(pos!=v.size() || x<=0) throw invalid_argument("positive integer required");
                if(key=="--n") n=x; else if(key=="--runs") runs=x; else if(key=="--block") requested_block=x; else timeout=x;
            } else throw invalid_argument("unknown option");
        }
        if(!n || out.empty() || (algo!="ikj" && algo!="blocked" && algo!="blocked_64" && algo!="blocked_128" && algo!="blocked_packed_64" && algo!="ikj_lda16")) throw invalid_argument("required options");
        if(algo=="blocked_packed_64" || algo=="ikj_lda16") { cerr << "未实现: " << algo << '\n'; return 2; }
        if ((algo=="blocked" && requested_block!=16 && requested_block!=32 && requested_block!=64 && requested_block!=128 && requested_block!=256) || (algo!="blocked" && requested_block)) throw invalid_argument("--block requires blocked and a supported size");
        int ld=n, block=algo=="blocked"?requested_block:algo=="blocked_64"?64:algo=="blocked_128"?128:0;
        if(n>46340) throw invalid_argument("n too large");
        size_t count=static_cast<size_t>(n)*ld;
        vector<int> a(count), b(count), c(count);
        if(!fill_inputs_bounded(a,b,n,ld)) { cerr << "未实现: fill_inputs_bounded TODO(零醛)\n"; return 2; }
        // The launcher enforces a hard process timeout; this deadline prevents starting another round.
        auto deadline=chrono::steady_clock::now()+chrono::seconds(timeout);
        ofstream csv(out); if(!csv) throw runtime_error("cannot open output");
        csv << "n,algorithm,pilot_ms,sample,repeats,total_ms,ms_per_multiply,C00,expected_C00,checksum,elementwise_match,status,ld,block_size,gmac_per_s,skipped_timeout\n";
        for(int sample=1; sample<=runs; ++sample) {
            if(chrono::steady_clock::now()>=deadline) { csv << n << ',' << algo << ",,,,,,,,,,," << ld << ',' << block << ",,1\n"; return 0; }
            auto start=chrono::steady_clock::now();
            fill(c.begin(),c.end(),0);
            bool ok=block ? matmul_blocked_ld(a,b,c,n,ld,block) : matmul_ikj_ld(a,b,c,n,ld);
            auto end=chrono::steady_clock::now();
            if(!ok) { cerr << "未实现: kernel TODO(零醛)\n"; return 2; }
            bool valid=true; long long checksum=0, expected00=0;
            for(int i=0;i<n;++i) for(int j=0;j<n;++j) {
                int64_t expected=0;
                for(int k=0;k<n;++k) expected+=int64_t(a[size_t(i)*ld+k])*b[size_t(k)*ld+j];
                if(i==0 && j==0) expected00=expected;
                if(expected!=c[size_t(i)*ld+j]) valid=false;
                checksum+=c[size_t(i)*ld+j];
            }
            double ms=chrono::duration<double,milli>(end-start).count();
            if(!valid || ms<=0) { cerr << "正确性检查失败或计时无效\n"; return 1; }
            double gmac=double(n)*n*n/(ms*1e6);
            csv << n << ',' << algo << ",," << sample << ",1," << ms << ',' << ms << ',' << c[0] << ',' << expected00 << ',' << checksum << ",YES,PASS," << ld << ',' << block << ',' << gmac << ",0\n";
            csv.flush();
        }
    } catch(const exception& e) { cerr << "参数或运行错误: " << e.what() << '\n'; return 2; }
    return 0;
}
