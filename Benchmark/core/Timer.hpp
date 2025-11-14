#ifndef __VELO_TIMER
#define __VELO_TIMER
#include <ctime>
#include <cstdint>

namespace VELO::Benchmark {

    class Timer {
        public:
        using TimeIntType = clock_t;
        using TimeType = timespec;
        private:
        static constexpr TimeIntType NANO = 1000000000LL;
        TimeType start_realtime{};
        TimeType start_cputime{};

        TimeType end_realtime{};
        TimeType end_cputime{};

        static uint64_t diff_ns(const timespec& a, const timespec& b) {
            return (uint64_t)(b.tv_sec - a.tv_sec) * NANO + (uint64_t)(b.tv_nsec - a.tv_nsec);
        }
        public:

        void start() {
            clock_gettime(CLOCK_MONOTONIC, &start_realtime);
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &start_cputime);
        }

        void end() {
            clock_gettime(CLOCK_MONOTONIC, &end_realtime);
            clock_gettime(CLOCK_THREAD_CPUTIME_ID, &end_cputime);
        }

        inline uint64_t cpu_time_used_ns() const { return diff_ns(start_cputime, end_cputime); }
        inline uint64_t real_time_used_ns() const { return diff_ns(start_realtime, end_realtime); }

        inline uint64_t cpu_time_used_us() const { return cpu_time_used_ns()/1000; }
        inline uint64_t real_time_used_us() const { return real_time_used_ns()/1000; }

        inline uint64_t cpu_time_used_ms() const { return cpu_time_used_ns()/1'000'000; }
        inline uint64_t real_time_used_ms() const { return real_time_used_ns()/1'000'000; }

        inline double cpu_time_used_sec_f() const { return static_cast<double>(cpu_time_used_ns()) / 1e9; }
        inline double real_time_used_sec_f() const { return static_cast<double>(real_time_used_ns()) / 1e9; }
        
        inline double cpu_time_used_ms_f() const { return static_cast<double>(cpu_time_used_ns()) / 1e6; }
        inline double real_time_used_ms_f() const { return static_cast<double>(real_time_used_ns()) / 1e6; }
    };
}

#endif