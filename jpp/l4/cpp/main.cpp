#include <iostream>
#include <format>
#include <cstdint>
#include <vector>
#include <mutex>
#include <semaphore>
#include <memory>
#include <algorithm>
#include <chrono>
#include <random>

constexpr auto MIN_THINKING = 300, MAX_THINKING = 500, MIN_EATING = 100, MAX_EATING = 250;

class sync_print {
    std::mutex print_mutex;
public:
    sync_print(const sync_print&) = delete;
    sync_print& operator=(const sync_print&) = delete;

    void operator()(std::string s) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << s;
    }
};

using Stats = struct {
    std::uint32_t n_eaten_meals;
};

using Constraints = struct {
    std::uint32_t n_meals_per_philo;
    std::uint32_t n_philo;
};

static std::mutex g_critial_section{};
static std::vector<Stats> g_stats;

static std::mutex flag_section{};
static std::vector<bool> flag_all_finished;

// Check if all philosophers can stop eating
bool is_finished() {
    std::lock_guard<std::mutex> lock(flag_section);
    return std::find(flag_all_finished.begin(), flag_all_finished.end(), false) == flag_all_finished.end();
} 

// Tag ith philo finished eating
void tag_finished_eating(std::uint32_t i) {
    std::lock_guard<std::mutex> lock(flag_section);
    flag_all_finished[i] = true;
}

std::chrono::milliseconds get_eating_time() {
    std::mt19937 engine(std::random_device{}());
    return std::chrono::milliseconds(std::uniform_int_distribution(MIN_EATING, MAX_EATING)(engine));
}

std::chrono::milliseconds get_thinking_time() {
    std::mt19937 engine(std::random_device{}());
    return std::chrono::milliseconds(std::uniform_int_distribution(MIN_THINKING, MAX_THINKING)(engine));
}

void eat(std::uint32_t i, sync_print& print) {
    auto time = get_eating_time();
    print(std::format("{} is eating for {}ms", i, time.count()));
    std::this_thread::sleep_for(time);
}


void philosopher(std::uint32_t i, const Constraints& constr, sync_print& print) {
    auto& stats = g_stats[i];
    bool untagged = false;

    while (!is_finished()) {




        if (!untagged && stats.n_eaten_meals == constr.n_meals_per_philo) {
            tag_finished_eating(i);
            untagged = true;
        }
    }
}

void run_problem(Constraints constraints) {

}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << std::format("Usage: {} <n_philosophers> <min_meals_per_philo>\n", argv[0]);
        std::cerr << std::format("Example: {} 5 30", argv[0]);
        return -1;
    }

    auto n_philo = std::atoi(argv[1]);
    auto n_meals = std::stoi(argv[2]);
    run_problem(Constraints {static_cast<std::uint32_t>(n_philo), static_cast<std::uint32_t>(n_meals)});
}