#ifndef __VELO_BENCHMARK
#define __VELO_BENCHMARK


#include "../../includes/Function.hpp"
#include "../../includes/HashMap.hpp"
#include "Reporter.hpp"
#include "State.hpp"
#include <limits>
#include <stdexcept>
#include <string>

namespace VELO::Benchmark {

    struct BenchmarkResult {
        Timer timer;
        size_t iterations;
        size_t processed_items;
        size_t processed_bytes;
    };

    class Benchmark {
        friend class BenchmarkInstance;
        friend class Runner;
        protected:
        Vector<Vector<int64_t>> args;
        std::string name;
        public:
        Benchmark(std::string const& name) : name(name) {}
        virtual void run(VELO::Benchmark::State& state) = 0;
    };
    
    class FunctionBenchmark : public Benchmark {
        friend class BenchmarkInstance;
        // using FuncType = VELO::Function<void(State&)>;
        typedef void(*FuncType)(State&);
        FuncType func;
        int64_t range_multiplier{8};

        static bool safe_mul(int64_t a, int64_t b, int64_t& out) {
            __int128 t = (__int128)a * (__int128)b;
            if(t > std::numeric_limits<int64_t>::max()) return false;
            out = (int64_t)t;
            return true;
        }
        
        public:

        FunctionBenchmark(std::string const& name, FuncType func) : Benchmark(name), func(func) {}

        void run(VELO::Benchmark::State& state) override { 
            func(state);
        }

        auto& push_args(const Vector<int64_t>& arg) {
            args.push_back(arg);
            return *this;
        }

        auto& push_range(int64_t lo, int64_t hi, int64_t multipler=8) {
            if(lo <= 0 || hi < lo || multipler < 2) throw std::invalid_argument("bad range");
            for (int64_t n = lo; n <= hi; n *= multipler)
                args.push_back({n});
            if (args.top()[0] != hi)
                args.push_back({hi});
            return *this;
        }

        
    };

    class BenchmarkInstance {
        friend class Runner;
        std::string name;
        Benchmark* bench;
        Vector<int64_t> args;

        public:
        
        explicit BenchmarkInstance(std::string const& name, const Vector<int64_t>& args, Benchmark* bench) : name(name), args(args), bench(bench) {}


        Benchmark* benchmark() { return bench; }
    };

    class Runner {
        Vector<Benchmark*> registry;

        BenchmarkResult _run_benchmark_impl(BenchmarkInstance& instance, size_t iters=1) {
            Timer timer;
            State state(timer, instance.benchmark(), iters);
            state.range = instance.args;
            timer.start();
            instance.benchmark()->run(state);
            timer.end();
            return {timer, iters, state.processed_items, state.processed_bytes};
        }
        public:
        
        auto& register_bench(Benchmark* bench) {
            registry.push_back(bench);
            return *this;
        }


        void run_benchmark(BenchmarkReporter& reporter) {
            Vector<BenchmarkReporterStats> stats;
            for(auto bench : registry) {
                for(auto& arg : bench->args) {
                    BenchmarkInstance instance(bench->name + "/" + std::to_string(arg.bottom()),arg, bench);

                    size_t iters = 1;
                    auto result = _run_benchmark_impl(instance, iters);

                    while(result.timer.real_time_used_ms() < 1000) {
                        iters *= 2;
                        result = _run_benchmark_impl(instance, iters);
                    }
                    stats.push_back({instance.name, result.timer, result.iterations, result.processed_items, result.processed_bytes});
                    printf("name: %s\nTmp cpu:%.2lfms/op\trealtime:%.2lfms/op\n", instance.name.c_str(), result.timer.cpu_time_used_ms_f()/result.iterations, result.timer.real_time_used_ms_f()/result.iterations);
                }
            }
            reporter.report_benchmarks(stats);
        }
    };
    
}
#endif