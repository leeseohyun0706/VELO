#ifndef __VELO_REPORTER
#define __VELO_REPORTER

#include "Timer.hpp"
#include "../../includes/Vector.hpp"
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

namespace VELO::Benchmark {
    
    struct BenchmarkReporterStats {
        std::string benchmark_name;
        Timer timer;
        size_t iterations;
        size_t processed_items;
        size_t processed_bytes;
    };

    class BenchmarkReporter {
        public:
        virtual ~BenchmarkReporter() = default;
        virtual void report_benchmarks(const Vector<BenchmarkReporterStats>& stats) = 0;
    };

    class ConsoleBenchmarkReporter : public BenchmarkReporter {
        struct TableLayout {
            size_t name{9};
            size_t iterations{10};
            size_t cpu_total{14};
            size_t real_total{15};
            size_t cpu_per{14};
            size_t real_per{15};
            size_t items_per_sec{13};
            size_t bytes_per_sec{13};
        };

        struct RowDisplay {
            std::string name;
            std::string iterations;
            std::string cpu_total;
            std::string real_total;
            std::string cpu_per_iter;
            std::string real_per_iter;
            std::string items_per_second;
            std::string bytes_per_second;
        };

        static std::string format_ms(double value) {
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(3) << value;
            return oss.str();
        }

        static size_t padded_width(size_t current, const std::string& value) {
            return std::max(current, value.size());
        }

        static std::string format_rate(double value, const char* unit) {
            std::ostringstream oss;
            static constexpr const char* prefixes[] = {"", "K", "M", "G", "T"};
            size_t prefix_index = 0;
            while(value >= 1000.0 && prefix_index + 1 < (sizeof(prefixes) / sizeof(prefixes[0]))) {
                value /= 1000.0;
                ++prefix_index;
            }
            oss << std::fixed << std::setprecision(1) << value << prefixes[prefix_index] << ' ' << unit;
            return oss.str();
        }

        static void print_divider(const TableLayout& layout, char fill='-') {
            auto segment = [&](size_t width) {
                std::cout << '+' << std::string(width + 2, fill);
            };
            segment(layout.name);
            segment(layout.iterations);
            segment(layout.cpu_total);
            segment(layout.real_total);
            segment(layout.cpu_per);
            segment(layout.real_per);
            segment(layout.items_per_sec);
            segment(layout.bytes_per_sec);
            std::cout << "+\n";
        }

        static void print_row(const TableLayout& layout, const RowDisplay& row) {
            auto cell = [](const std::string& v, size_t width, bool align_left=false) {
                std::cout << "| ";
                if(align_left) {
                    std::cout << std::left << std::setw(width) << v << std::right;
                } else {
                    std::cout << std::setw(width) << v;
                }
                std::cout << ' ';
            };

            cell(row.name, layout.name, true);
            cell(row.iterations, layout.iterations);
            cell(row.cpu_total, layout.cpu_total);
            cell(row.real_total, layout.real_total);
            cell(row.cpu_per_iter, layout.cpu_per);
            cell(row.real_per_iter, layout.real_per);
            cell(row.items_per_second, layout.items_per_sec);
            cell(row.bytes_per_second, layout.bytes_per_sec);
            std::cout << "|\n";
        }

        std::vector<RowDisplay> build_rows(const Vector<BenchmarkReporterStats>& stats, TableLayout& layout) const {
            std::vector<RowDisplay> rows;
            rows.reserve(stats.size());
            for(size_t i = 0; i < stats.size(); ++i) {
                const auto& stat = stats.at(i);
                RowDisplay row;
                row.name = stat.benchmark_name;
                row.iterations = std::to_string(stat.iterations);

                double cpu_total_ms = stat.timer.cpu_time_used_ms_f();
                double real_total_ms = stat.timer.real_time_used_ms_f();
                double iter_div = stat.iterations == 0 ? 1.0 : static_cast<double>(stat.iterations);
                double real_seconds = std::max(stat.timer.real_time_used_sec_f(), std::numeric_limits<double>::epsilon());

                row.cpu_total = format_ms(cpu_total_ms);
                row.real_total = format_ms(real_total_ms);
                row.cpu_per_iter = format_ms(cpu_total_ms / iter_div);
                row.real_per_iter = format_ms(real_total_ms / iter_div);
                row.items_per_second = format_rate(static_cast<double>(stat.processed_items) / real_seconds, "items/s");
                row.bytes_per_second = format_rate(static_cast<double>(stat.processed_bytes) / real_seconds, "bytes/s");

                layout.name = padded_width(layout.name, row.name);
                layout.iterations = padded_width(layout.iterations, row.iterations);
                layout.cpu_total = padded_width(layout.cpu_total, row.cpu_total);
                layout.real_total = padded_width(layout.real_total, row.real_total);
                layout.cpu_per = padded_width(layout.cpu_per, row.cpu_per_iter);
                layout.real_per = padded_width(layout.real_per, row.real_per_iter);
                layout.items_per_sec = padded_width(layout.items_per_sec, row.items_per_second);
                layout.bytes_per_sec = padded_width(layout.bytes_per_sec, row.bytes_per_second);

                rows.push_back(std::move(row));
            }
            return rows;
        }

        public:
        void report_benchmarks(const Vector<BenchmarkReporterStats>& stats) override {
            if(stats.size() == 0) {
                std::cout << "[benchmark] 측정할 항목이 없습니다.\n";
                return;
            }

            TableLayout layout{
                std::string("Benchmark").size(),
                std::string("Iterations").size(),
                std::string("CPU total (ms)").size(),
                std::string("Real total (ms)").size(),
                std::string("CPU / iter (ms)").size(),
                std::string("Real / iter (ms)").size(),
                std::string("Items / sec").size(),
                std::string("Bytes / sec").size()
            };

            auto rows = build_rows(stats, layout);

            RowDisplay header{
                "Benchmark",
                "Iterations",
                "CPU total (ms)",
                "Real total (ms)",
                "CPU / iter (ms)",
                "Real / iter (ms)",
                "Items / sec",
                "Bytes / sec"
            };

            print_divider(layout, '=');
            print_row(layout, header);
            print_divider(layout);
            for(const auto& row : rows) {
                print_row(layout, row);
            }
            print_divider(layout, '=');
        }
    };
}

#endif 
