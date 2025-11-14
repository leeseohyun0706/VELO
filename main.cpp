

#include "../VELO/includes/HashMap.hpp"
#include "../VELO/includes/Tree.hpp"
#include "../VELO/includes/Iterator.hpp"
#include "Benchmark/core/Reporter.hpp"
#include "includes/Array.hpp"
#include "includes/Function.hpp"
#include "includes/Tuple.hpp"
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>
#include "Benchmark/core/Benchmark.hpp"

using namespace VELO;
using namespace VELO::Benchmark;

void rstp(int l1, int l2, int l3) {
    printf("%d\n", l1+l2+l3);
}

void my_bench(State& state) {
    auto N = state.collect_range(0);
    VELO::HashMap<std::string, int, Hash<std::string>, Basic_Template_Allocator<__hash_map_node<std::string, int>>> cmap(N);
    while(state.keep_running()) {
        for(int i = 0; i < N; i++) {
            cmap.insert("Fuck"+std::to_string(i), 2676);
            cmap.insert("느금"+std::to_string(i), 6466);
            cmap.insert("Fu3ck"+std::to_string(i), 2676);
            cmap.insert("느2금"+std::to_string(i), 6466);
            cmap.insert("Fuc0k"+std::to_string(i), 2676);
            cmap.insert("느9금"+std::to_string(i), 6466);
            cmap.insert("Fu3ck"+std::to_string(i), 2676);
            cmap.insert("느금111"+std::to_string(i), 6466);
            state.item_process(8);
            cmap.erase("Fu3ck"+std::to_string(i));
            cmap.erase("느9금"+std::to_string(i));
            state.item_process(2);
        }
    }
}

void stl_bench(State& state) {
    auto N = state.collect_range(0);
    std::unordered_map<std::string, int, Hash<std::string>> cmap;
    cmap.reserve(N);
    while(state.keep_running()) {
        for(int i = 0; i < N; i++) {
            cmap.insert({"Fuck"+std::to_string(i), 2676});
            cmap.insert({"느금"+std::to_string(i), 6466});
            cmap.insert({"Fu3ck"+std::to_string(i), 2676});
            cmap.insert({"느2금"+std::to_string(i), 6466});
            cmap.insert({"Fuc0k"+std::to_string(i), 2676});
            cmap.insert({"느9금"+std::to_string(i), 6466});
            cmap.insert({"Fu3ck"+std::to_string(i), 2676});
            cmap.insert({"느금111"+std::to_string(i), 6466});
            state.item_process(8);

            cmap.erase("Fu3ck"+std::to_string(i));
            cmap.erase("느9금"+std::to_string(i));
            state.item_process(2);
        }
    }
}
int main() {
    ConsoleBenchmarkReporter reporter;
    Runner runner;
    FunctionBenchmark my("myBench", my_bench);
    FunctionBenchmark stl("stlBench", stl_bench);

    my.push_args({1024*1024});
    stl.push_args({1024*1024});
    // my.push_range(256, 256*1000);
    // stl.push_range(256, 256*1000);
    
    runner.register_bench(&my);
    runner.register_bench(&stl);
 
    runner.run_benchmark(reporter);
    return 0;

    Function<void(int,int,int)> fc(rstp);
    static constexpr int N = 200000;

    for(int i = 0; i < 100; i++) {
        fc(i, i, i);
    }
    Timer a,b;
    
    a.start();
    {
    VELO::HashMap<std::string, int, Hash<std::string>, Basic_Template_Allocator<__hash_map_node<std::string, int>>> cmap(N);
        
    for(int i = 0; i < N; i++) {
        cmap.insert("Fuck"+std::to_string(i), 2676);
        cmap.insert("느금"+std::to_string(i), 6466);
        cmap.insert("Fu3ck"+std::to_string(i), 2676);
        cmap.insert("느2금"+std::to_string(i), 6466);
        cmap.insert("Fuc0k"+std::to_string(i), 2676);
        cmap.insert("느9금"+std::to_string(i), 6466);
        cmap.insert("Fu3ck"+std::to_string(i), 2676);
        cmap.insert("느금111"+std::to_string(i), 6466);

        cmap.erase("Fu3ck"+std::to_string(i));
        cmap.erase("느9금"+std::to_string(i));
    }
    }
    a.end();

    b.start();
{
    std::unordered_map<std::string, int, Hash<std::string>> cmap;
    cmap.reserve(N);
    for(int i = 0; i < N; i++) {
        cmap.insert({"Fuck"+std::to_string(i), 2676});
        cmap.insert({"느금"+std::to_string(i), 6466});
        cmap.insert({"Fu3ck"+std::to_string(i), 2676});
        cmap.insert({"느2금"+std::to_string(i), 6466});
        cmap.insert({"Fuc0k"+std::to_string(i), 2676});
        cmap.insert({"느9금"+std::to_string(i), 6466});
        cmap.insert({"Fu3ck"+std::to_string(i), 2676});
        cmap.insert({"느금111"+std::to_string(i), 6466});

        cmap.erase("Fu3ck"+std::to_string(i));
        cmap.erase("느9금"+std::to_string(i));
    }
}

b.end();
    printf("C : %lfms\nL : %lfms\n", a.cpu_time_used_ms_f(), b.cpu_time_used_ms_f());

    Tuple<int, std::string, char, float> fsc(34, "fdss", 'c', 32.f);

    printf("%s\n", get<1>(fsc).c_str());    
    // srand(time(0));
    // RBSet<size_t> rset;
    // for(int i = 0; i < 100; ++i) {
    //     rset.insert(rand()%10000);
    // }

    // VELO::for_each(rset.begin(), rset.end(), [](size_t ls) {
    //     printf("%lu\n", ls);
    // });
    // for(auto& r : rset) printf("%lu\n", r);
    // __rb_tree<std::string> sset;
    // sset.insert("니애미");
    // sset.insert("니애비");
    // sset.insert("니할매");
    // sset.insert("니할배");

    // for(auto& i : sset) {
    //     printf("%s\n", i.c_str());
    // }
    
    // sset.erase("니애미");

    // for(auto& i : sset) {
    //     printf("%s\n", i.c_str());
    // }

    // __rb_tree<int> fvec;
    // fvec.insert(362);
    // fvec.insert(3692);
    // fvec.insert(3362);
    // fvec.insert(36872);

    // for(auto& i : fvec) {
    //     printf("%d\n", i);
    // }
}
