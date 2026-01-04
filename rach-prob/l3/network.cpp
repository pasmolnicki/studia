#include <format>
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <concepts>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <filesystem>
#include <fstream>

// Simple thread pool
class thread_pool {
public:
    using task_t = std::function<void()>;
    using task_queue_t = std::vector<task_t>;
    using worker_threads_t = std::vector<std::thread>;

private:
    worker_threads_t workers;
    std::atomic_bool stop{false};
    std::mutex queue_mutex;
    std::condition_variable condition;
    task_queue_t tasks;

    std::atomic<int> completed_tasks = 0;
    int total_tasks = 0;

public:
    thread_pool(size_t threads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < threads; ++i) {
            workers.emplace_back([this, i](){
                while(true) {
                    task_t task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this] { return stop || !tasks.empty(); });
                        if (stop && tasks.empty()) return;
                        task = std::move(tasks.back());
                        tasks.pop_back();
                    }
                    task();
                    completed_tasks++;
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
        int prev_completed = -1;

        while (true)
        {
            auto completed = completed_tasks.load();
            if (completed != prev_completed) {
                prev_completed = completed;
                std::cout << std::format("Completed tasks: {}/{}\n", completed, total_tasks);
            }

            if (completed >= total_tasks) break;
            std::this_thread::yield();
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

    void enqueue(task_t&& f) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            tasks.emplace_back(std::forward<task_t>(f));
        }
        condition.notify_one();
    }
};



template <typename T>
concept RandomAccessContainer = requires(T a, T b) {
    { a < b } -> std::convertible_to<bool>;
    { a = b } -> std::same_as<T&>;
    { a[0] } -> std::convertible_to<typename T::value_type&>;
    { a.size() } -> std::convertible_to<size_t>;
};

// Simulates a star topology network trial, with one node activating others
// with probability p. Returns the number of trials (cycles) until all nodes are activated.
[[nodiscard]] size_t 
network_trial(size_t n, std::mt19937& generator, double p) noexcept(true) {
    std::vector<bool> nodes(n, false);
    std::uniform_real_distribution<double> dist{0.0, 1.0};
    size_t activated_count{0};
    size_t trials{0};

    while (activated_count < n) {
        trials++;
        for (size_t i{0}; i < n; i++) {
            if (!nodes[i]) {
                if (dist(generator) < p) {
                    nodes[i] = true;
                    activated_count++;
                }
            }
        }
    }

    return trials;
}

constexpr size_t SIMULATIONS = 50,
                 N_START = 1000,
                 N_END = 100000,
                 N_STEP = 1000;

constexpr double ACTIVATION_PROBABILITY = 0.1;
constexpr auto OUTPUT_FOLDER = "network/",
                FILE_FMT = "network_{}_{:.1f}.csv",
                CSV_HEADER = "n;p;trials\n";


void run_simulation(int id, size_t n, size_t simulations, double p, size_t seed) {
    auto file_path = std::filesystem::path(OUTPUT_FOLDER) / 
                     std::format(FILE_FMT, id, p);

    std::cout << std::format(
        "Task {}: Running network simulations for n={} with p={} and seed={}...\n", 
        id, n, p, seed);

    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }
    file << CSV_HEADER;

    std::vector<std::string> lines;
    lines.reserve(simulations);
    std::mt19937 generator(static_cast<std::mt19937::result_type>(seed));

    for (size_t k = 0; k < simulations; k++) {
        auto trials = network_trial(n, generator, p);
        lines.push_back(std::format("{};{};{}\n", n, p, trials));
    }

    for (const auto& line : lines) {
        file << line;
    }
}


int main(int argc, char** argv) {
    double p = ACTIVATION_PROBABILITY;
    if (argc == 2) {
        // Try to parse it as a double (activation probability)
        try {
            p = std::stod(argv[1]);
            std::cout << "Using activation probability p = " << p << " for simulations.\n";
        } catch (const std::exception& e) {
            std::cerr << "Invalid argument for activation probability. Using default p=0.1.\n";
            p = ACTIVATION_PROBABILITY;
        }
    }
    
    std::cout << "Creating output directory...\n";
    std::filesystem::create_directory(OUTPUT_FOLDER);

    thread_pool pool;
    size_t n_values = (N_END - N_START) / N_STEP + 1;
    pool.set_n_tasks(n_values);

    std::random_device rd{};
    int task_id = 0;
    for (size_t n = N_START; n <= N_END; n += N_STEP) {
        auto seed = rd();
        task_id++;
        pool.enqueue([task_id, n, p, seed]() {
            run_simulation(task_id, n, SIMULATIONS, p, seed);
        });
    }

    pool.join();
    return 0;
}