#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <functional>
#include <format>
#include <string>
#include <fstream>
#include <string_view>
#include <filesystem>
#include <thread>
#include <condition_variable>

/*

Compile with c++20 standard (for std::format):
g++ -std=c++20 -O3 -o main main.cpp

m - # of balls
n - # of bins

Bn : First collision
Un : # empty bins after n balls
Cn : min # balls, so that there are no empty bins
Dn : min # balls, so that there are at least 2 balls in each bin
Dn - Cn : # balls to from 1 to 2 in each bin
Ln : max load after n balls

Simulate with n [1000, 2000, ..., 100 000]
k = 50 repetitions

*/
constexpr const char* OUTPUT_DIR = "./outputs";
constexpr const char* FILE_NAME_FMT = "{}/outputs{}.csv";
constexpr auto SIM_REPETITIONS = 50;

typedef int number_t;
typedef std::vector<number_t> vec_t;
typedef struct stats_t {
    number_t first_collision{0};
    number_t n_empty_bins_after_n{0};
    number_t min_balls_no_empty_bin{0};
    number_t min_balls_each_2_in_bin{0};
    number_t n_balls_from_1_to_2{0};
    number_t max_load_d_after_n{0};

    static constexpr std::string_view csv_header() {
        return "first_collision;n_empty_bins_after_n;min_balls_no_empty_bin;min_balls_each_2_in_bin;n_balls_from_1_to_2;max_load_d_after_n";
    }

    inline std::string to_csv() const {
        return std::format("{};{};{};{};{};{}",
            first_collision,
            n_empty_bins_after_n,
            min_balls_no_empty_bin,
            min_balls_each_2_in_bin,
            n_balls_from_1_to_2,
            max_load_d_after_n);
    }
} stats_t;

std::ostream& operator<<(std::ostream& out, const stats_t& stats) {
    return out << "First collision: " << stats.first_collision << ", "
            << "Empty bins after n balls: " << stats.n_empty_bins_after_n << ", "
            << "Min balls no empty bin: " << stats.min_balls_no_empty_bin << ", "
            << "Min balls each 2 in bin: " << stats.min_balls_each_2_in_bin << ", "
            << "Balls from 1 to 2 in each bin: " << stats.n_balls_from_1_to_2;
}

stats_t sim_balls_and_bins(number_t n_bins, number_t d, std::mt19937& local_gen) {
    auto bins = vec_t(n_bins, 0);
    auto stats = stats_t{};
    std::uniform_int_distribution<number_t> dist(0, n_bins);

    // Optimized empty bin count
    auto empty_bins = n_bins;
    auto bins_with_more_than_1 = 0;

    for (number_t ball_no = 1; stats.min_balls_each_2_in_bin == 0; ball_no++) {
        
        // Choose one bin, least full from "d" bins (with rep)
        // uniformly from [0, n_bins-1]
        number_t chosen = bins[0];
        number_t chosen_idx = 0;
        for (number_t i = 0; i < d; i++) {
            auto candidate = dist(local_gen);
            if (i == 0 || bins[candidate] < bins[chosen]) {
                chosen = bins[candidate];
                chosen_idx = candidate;
            }
        }

        // Place ball in chosen bin
        bins[chosen_idx]++;
        chosen = bins[chosen_idx];

        // This bin was empty before
        if (chosen == 1) {
            empty_bins--;
        }
        // This bin has now more than 1 ball
        if (chosen == 2) {
            bins_with_more_than_1++;
        }

        // Max load after n balls (simply keep tracking until n balls)
        if (ball_no <= n_bins && chosen > stats.max_load_d_after_n) {
            stats.max_load_d_after_n = chosen;
        }

        // U(n) Count non-empty bins after n balls
        if (ball_no == n_bins) {
            stats.n_empty_bins_after_n = empty_bins;
        }

        // B(n) First collision
        if (chosen > 1 && stats.first_collision == 0) {
            stats.first_collision = ball_no;
        }
        // C(n) The moment, when there's no empty balls
        if (empty_bins == 0 && stats.min_balls_no_empty_bin == 0) {
            stats.min_balls_no_empty_bin = ball_no;
        }
        // D(n) In each bin, there are 2 balls, the loop will break after this 
        // condition is met
        if (bins_with_more_than_1 == n_bins) {
            stats.min_balls_each_2_in_bin = ball_no;
            stats.n_balls_from_1_to_2 = ball_no - stats.min_balls_no_empty_bin;
        }
    }

    return stats;
}

void run_experiment(int k, int d, size_t seed) {

    constexpr auto BINS_START = 1e4;
    constexpr auto BINS_END = 1e5;
    constexpr auto BINS_STEP = 1e3;

    std::fstream file;
    auto file_path = std::format(FILE_NAME_FMT, OUTPUT_DIR, k);
    file.open(file_path, std::ios::out);

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }

    std::mt19937 local_gen(seed);
    file << "n_bins;" << stats_t::csv_header() << "\n";
    std::vector<std::string> lines;
    lines.reserve((BINS_END - BINS_START) / BINS_STEP + 1);

    for (number_t n_bins = BINS_START; n_bins <= BINS_END; n_bins += BINS_STEP) {
        auto stats = sim_balls_and_bins(n_bins, d, local_gen);
        lines.push_back(std::format("{};{}\n", n_bins, stats.to_csv()));
    }

    // Write all lines at once
    for (const auto& line : lines) {
        file << line;
    }
    file.close();
}

// Simple thread pool
class thread_pool {
    std::vector<std::thread> workers;
    std::atomic_bool stop{false};
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::vector<std::function<void()>> tasks;

    int completed_tasks = 0;
    int total_tasks = 0;
    std::mutex progress_mutex;

public:
    thread_pool(size_t threads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this, i](){
                while(true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.back());
                        tasks.pop_back();
                    }
                    task();
                    {
                        std::unique_lock<std::mutex> lock(progress_mutex);
                        completed_tasks++;
                        std::cout << "Thread " << i << " completed task " << completed_tasks << "/" << total_tasks << "\n";
                    }
                }
            });
        }
    }

    ~thread_pool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        join();
    }

    void set_n_tasks(size_t n) {
        tasks.reserve(n);
        total_tasks = n;
    }

    void join() {
        if (total_tasks == 0) return;

        while (true)
        {
            {
                std::unique_lock<std::mutex> lock(progress_mutex);
                if (completed_tasks >= total_tasks) break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            stop = true;
        }
        condition.notify_all();
        for (std::thread &worker: workers) {
            if (worker.joinable()) {
                worker.join();
            }
        }
    }

    template<std::invocable F>
    void enqueue(F&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace_back(std::forward<F>(f));
        }
        condition.notify_one();
    }
};

int main(int argc, char** argv) {
    int d = 1;
    if (argc == 2) {
        // Try to parse it as an integer (d value)
        try {
            d = std::stoi(argv[1]);
            std::cout << "Using d = " << d << " for simulations.\n";
        } catch (const std::exception& e) {
            std::cerr << "Invalid argument for d value. Using default d=1.\n";
            d = 1;
        }
    }

    std::cout << "Creating output directory...\n";
    std::filesystem::create_directory(OUTPUT_DIR);

    std::cout << "Starting simulations...\n";
    std::random_device rd{};
    thread_pool pool;
    pool.set_n_tasks(SIM_REPETITIONS);
    
    std::chrono::time_point<std::chrono::high_resolution_clock> 
        start_time = std::chrono::high_resolution_clock::now();
    for (int k = 1; k <= SIM_REPETITIONS; k++) {
        auto seed = rd();
        std::cout << "Launching simulation " << k << " (seed=" << seed << ")...\n";
        pool.enqueue([k, d, seed]() {
            run_experiment(k, d, seed);
        });
    }

    pool.join();
    std::cout << std::format("All simulations completed in {:.2f} seconds.\n",
        std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - start_time).count());
    return 0;
}
