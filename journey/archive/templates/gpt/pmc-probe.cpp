// AI 维护脚手架；TODO(零醛) 由零醛实现，不是 AI 代写的学生实现。
// g++ -std=c++17 -O2 templates/gpt/pmc-probe.cpp -o pmc-probe
// 固定 Day 7 容量 n<=256；初始化包含在进程内，repeats=0 只初始化。
#define main day07_benchmark_main
#include "../../src/day07-matrix-blocking.cpp"
#undef main
#include <string>
#include <stdexcept>
#include <chrono>
#include <fstream>
// TODO(零醛)：算术量与 real 相同，访问数据常驻缓存；正确性与公平性需另行验证。
bool matmul_resident(int, const std::string&) { return false; }
int main(int argc,char** argv) {
    try {
        if(argc==2 && std::string(argv[1])=="--help") { std::cout<<"<ikj|blocked> <n> <repeats> --variant <real|resident> [--out CSV]\n"; return 0; }
        if(argc!=6 && argc!=8) throw std::invalid_argument("invalid arguments");
        std::string algo=argv[1], variant=argv[5],out; size_t pos=0;
        int n=std::stoi(argv[2],&pos); if(pos!=std::string(argv[2]).size()) throw std::invalid_argument("invalid n");
        int repeats=std::stoi(argv[3],&pos); if(pos!=std::string(argv[3]).size()) throw std::invalid_argument("invalid repeats");
        if(std::string(argv[4])!="--variant" || (variant!="real" && variant!="resident") || (algo!="ikj" && algo!="blocked") || n<1 || n>MAX_N || repeats<0) throw std::invalid_argument("invalid options");
        if(argc==8) { if(std::string(argv[6])!="--out") throw std::invalid_argument("invalid output"); out=argv[7]; }
        initialize_inputs(n);
        if(repeats==0) { std::cout<<"initialization only\n"; return 0; }
        if(variant=="resident" && !matmul_resident(n,algo)) { std::cerr<<"未实现: resident TODO(零醛)\n"; return 2; }
        auto t0=std::chrono::steady_clock::now();
        for(int r=0;r<repeats;++r) {
            if(variant=="resident") { if(!matmul_resident(n,algo)) return 2; }
            else if(algo=="ikj") matmul_ikj(n); else matmul_blocked(n,64);
        }
        auto t1=std::chrono::steady_clock::now();
        double ms=std::chrono::duration<double,std::milli>(t1-t0).count()/repeats;
        auto checksum=checksum_matrix(n); std::cout<<"checksum="<<checksum<<'\n';
        if(!out.empty()) { std::ofstream csv(out); if(!csv) throw std::runtime_error("cannot open CSV"); csv<<"algo,n,variant,repeats,ms_per_multiply,checksum\n"<<algo<<','<<n<<','<<variant<<','<<repeats<<','<<ms<<','<<checksum<<'\n'; }
    } catch(const std::exception& e) { std::cerr<<e.what()<<'\n'; return 2; }
}
