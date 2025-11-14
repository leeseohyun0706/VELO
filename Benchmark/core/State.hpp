#ifndef __VELO_STATE
#define __VELO_STATE
#include "../../includes/Vector.hpp"
// #include "Benchmark.hpp"
#include "Timer.hpp"

namespace VELO::Benchmark {

    class State {
        friend class BenchmarkInstance;
        friend class Runner;

        Vector<int64_t> range{};
        size_t iterations{};
        size_t max_iterations{};
        Timer& timer;
        
        size_t processed_items{0};
        size_t processed_bytes{0};

        class Benchmark* bench;
        
        public:
        State(Timer& timer, Benchmark* bench, size_t max_iterations=1) : timer(timer), bench(bench), max_iterations(max_iterations) {}

        int64_t collect_range(int i) { return range[i]; }


        bool keep_running() { return iterations++ < max_iterations; }
        
        inline void item_process(size_t n) { processed_items+=n; }
        inline void byte_process(size_t b) { processed_bytes+=b; }
    };
}
#endif