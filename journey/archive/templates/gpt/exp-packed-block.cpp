// AI 维护脚手架；TODO(零醛) 由零醛本人实现，不是 AI 代写的学生实现。
// 构建：g++ -std=c++17 -O2 templates/gpt/exp-packed-block.cpp -o packed
// 运行：packed --help；仅 n<=256，正式测量须先验证规则与正确性。
#define main day07_benchmark_main
#include "../../src/day07-matrix-blocking.cpp"
#undef main
#include <stdexcept>
#include <string>
#include <chrono>
#include <fstream>
// TODO(零醛)：实现 B 子块拷贝；若用 memcpy，须在报告中注明。
bool pack_B_block(int /*k*/, int /*j*/, int /*b*/, int* /*packed*/) { return false; }
// TODO(零醛)：实现打包分块，包含 C 清零。
bool matmul_blocked_packed(int /*n*/, int /*b*/) { return false; }
#ifndef PACKED_BLOCK_TEST
int main(int argc,char** argv) {
    int n=0,b=0,runs=0; std::string algo,out;
    try {
        for(int i=1;i<argc;++i) {
            std::string key=argv[i];
            if(key=="--help") { std::cout << "--algo {ikj,blocked,blocked_packed,pack_only} --n {128,256} --b {32,64} --runs N --out CSV\n"; return 0; }
            if(i+1>=argc) throw std::invalid_argument("missing value");
            std::string v=argv[++i]; size_t pos=0;
            if(key=="--algo") algo=v; else if(key=="--out") out=v;
            else if(key=="--n" || key=="--b" || key=="--runs") {
                int x=std::stoi(v,&pos); if(pos!=v.size() || x<=0) throw std::invalid_argument("invalid integer");
                if(key=="--n") n=x; else if(key=="--b") b=x; else runs=x;
            } else throw std::invalid_argument("unknown option");
        }
        if((n!=128 && n!=256) || (b!=32 && b!=64) || runs<=0 || out.empty() || (algo!="ikj" && algo!="blocked" && algo!="blocked_packed" && algo!="pack_only")) throw std::invalid_argument("invalid options");
        initialize_inputs(n);
        // Avoid creating a misleading result file when either learner function is incomplete.
        alignas(64) static int probe[64*64]={};
        if(algo=="pack_only" && !pack_B_block(0,0,b,probe)) { std::cerr << "未实现: pack_B_block TODO(零醛)\n"; return 2; }
        if(algo=="blocked_packed" && !matmul_blocked_packed(n,b)) { std::cerr << "未实现: matmul_blocked_packed TODO(零醛)\n"; return 2; }
        std::ofstream csv(out); if(!csv) throw std::runtime_error("cannot open output");
        csv << "n,b,algorithm,sample,ms,checksum\n";
        for(int r=1;r<=runs;++r) {
            double ms=0; long long checksum=0;
            if(algo=="pack_only") {
                alignas(64) static int buffer[64*64]={};
                for(int k=0;k<n;k+=b) for(int j=0;j<n;j+=b) {
                    auto t0=std::chrono::steady_clock::now();
                    bool ok=pack_B_block(k,j,b,buffer);
                    auto t1=std::chrono::steady_clock::now();
                    if(!ok) { std::cerr << "未实现: pack_B_block\n"; return 2; }
                    ms+=std::chrono::duration<double,std::milli>(t1-t0).count();
                    // Read every packed element after each timed copy, so the writes remain observable.
                    for(int q=0;q<b*b;++q) checksum+=buffer[q];
                }
            } else {
                auto t0=std::chrono::steady_clock::now();
                if(algo=="ikj") matmul_ikj(n); else if(algo=="blocked") matmul_blocked(n,b);
                else if(!matmul_blocked_packed(n,b)) { std::cerr << "未实现: matmul_blocked_packed\n"; return 2; }
                auto t1=std::chrono::steady_clock::now();
                ms=std::chrono::duration<double,std::milli>(t1-t0).count(); checksum=checksum_matrix(n);
            }
            csv << n << ',' << b << ',' << algo << ',' << r << ',' << ms << ',' << checksum << '\n';
            std::cout << "checksum=" << checksum << '\n';
        }
    } catch(const std::exception& e) { std::cerr << e.what() << '\n'; return 2; }
    return 0;
}

#endif
