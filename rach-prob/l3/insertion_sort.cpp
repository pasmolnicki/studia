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

// Insertion sort algorithm that sorts the container in place
// and returns the number of comparisons made and number of swaps performed.
template <RandomAccessContainer T>
[[nodiscard]] std::pair<typename T::value_type, typename T::value_type> 
insertion_sort(T& arr) noexcept {
    typename T::value_type comparsions{0};
    typename T::value_type n_swaps{0};
    for (size_t i = 1; i < arr.size(); i++) {
        int key = arr[i];
        auto j = i;
        while (j > 0) {
            comparsions++;
            if (arr[j - 1] <= key) {
                break;
            }

            arr[j] = arr[j - 1];
            j--;
            n_swaps++;
        }
        arr[j] = key;
    }
    return {comparsions, n_swaps};
}

// For debugging: prints the contents of a vector to an output stream.
std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec) {
    os << "[";
    for (size_t i = 0; i < vec.size(); i++) {
        os << vec[i];
        if (i != vec.size() - 1) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}

// Generates a random permutation of integers from 1 to n using the provided generator.
template <std::integral T>
void permutation(
    typename std::vector<T>::iterator begin, 
    typename std::vector<T>::iterator end, 
    std::mt19937& generator,
    T n
) noexcept {
    T i = 1;
    std::generate_n(begin, n, [&]() { return i++; });
    std::shuffle(begin, end, generator);
}


constexpr std::string_view csv_header() {
    return "n;comparisons;n_swaps\n";
}

constexpr auto OUTPUT_FOLDER = "insertion-sort/",
               FILE_FMT = "outputs_{}.csv";

constexpr int SIMULATIONS = 50;

void run_simulation(int id, int n, size_t seed) {
    auto file_path = std::filesystem::path(OUTPUT_FOLDER) / 
                     std::format(FILE_FMT, id);

    std::cout << std::format(
        "Task {}: Running simulations for n={} with seed={}...\n", 
        id, n, seed);

    std::ofstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return;
    }
    file << csv_header();

    std::vector<std::string> lines;
    lines.reserve(SIMULATIONS);
    std::vector<int> arr(n, 0);
    std::mt19937 generator(static_cast<std::mt19937::result_type>(seed));

    // Run k permutations and sort each, recording the number of comparisons
    for (int k = 0; k < SIMULATIONS; k++) {
        permutation(arr.begin(), arr.end(), generator, n);
        auto [comparisons, n_swaps] = insertion_sort(arr);
        lines.push_back(std::format("{};{};{}\n", n, comparisons, n_swaps));
    }

    // Write all lines at once
    for (const auto& line : lines) {
        file << line;
    }
}


int main() {
    constexpr int N_BEGIN = 100;
    constexpr int N_STEP = 100;
    constexpr int N_END = 10000;
    constexpr auto N_VALUES = (N_END - N_BEGIN) / N_STEP + 1;

    std::filesystem::create_directory(OUTPUT_FOLDER);
    std::random_device rd{};
    thread_pool pool;

    pool.set_n_tasks(N_VALUES);
    int task_id = 0;
    for (int n = N_BEGIN; n <= N_END; n += N_STEP) {
        auto seed = rd();
        task_id++;
        pool.enqueue([task_id, n, seed]() {
            run_simulation(task_id, n, seed);
        });
    }

    pool.join();
    return 0;
}