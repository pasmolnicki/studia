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
#include <condition_variable>
#include <ranges>

constexpr auto MIN_THINKING = 300, MAX_THINKING = 500,
               MIN_EATING = 100, MAX_EATING = 250;

class sync_print {
    std::mutex print_mutex;
public:
    sync_print() = default;
    sync_print(const sync_print&) = delete;
    sync_print& operator=(const sync_print&) = delete;

    void operator()(std::string s) {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << std::format("[debug] {}", s);
    }
};

using Stats = struct {
    std::uint32_t n_eaten_meals{0};
};

using Constraints = struct {
    std::uint32_t n_meals_per_philo{5};
    std::uint32_t n_philo{5};
};

enum class PhiloState {
    HUNGRY,
    EATING,
    THINKING,
};

static std::mutex critial_section{};
static std::vector<Stats> stats;
static std::vector<std::unique_ptr<std::condition_variable>> cv;
static std::vector<PhiloState> state;

static std::mutex flag_section{};
static std::vector<bool> flag_all_finished;

std::uint32_t right_index(std::uint32_t i) {
    return (i + 1) % static_cast<std::uint32_t>(cv.size());
}

std::uint32_t left_index(std::uint32_t i) {
    return (i + cv.size() - 1) % static_cast<std::uint32_t>(cv.size());
}

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

template<typename T>
T get_random(int min, int max) {
    std::mt19937 engine(std::random_device{}());
    return T(std::uniform_int_distribution(min, max)(engine));
}

std::chrono::microseconds get_eating_time() {
    return get_random<std::chrono::microseconds>(MIN_EATING, MAX_EATING);
}

std::chrono::microseconds get_thinking_time() {
    return get_random<std::chrono::microseconds>(MIN_THINKING, MAX_THINKING);
}

void eat(std::uint32_t i, sync_print& print) {
    auto time = get_eating_time();
    print(std::format("{} is eating for {}\n", i, time));
    std::this_thread::sleep_for(time);
    stats[i].n_eaten_meals++;
}

void think(std::uint32_t i, sync_print& print) {
    auto time = get_thinking_time();
    print(std::format("{} is thinking for {}\n", i, time));
    std::this_thread::sleep_for(time);
}

void take_forks(std::uint32_t i, sync_print& print) {
    std::unique_lock lock(critial_section);

    state[i] = PhiloState::HUNGRY;

    cv[i]->wait(lock, [=](){
        return state[left_index(i)] != PhiloState::EATING && state[right_index(i)] != PhiloState::EATING;
    });

    state[i] = PhiloState::EATING;
}

void put_forks(std::uint32_t i, sync_print& print) {
    std::unique_lock lock(critial_section);

    state[i] = PhiloState::THINKING;

    cv[left_index(i)]->notify_one();
    cv[right_index(i)]->notify_one();
}

void philosopher(std::uint32_t i, const Constraints& constr, sync_print& print) {
    bool untagged = false;

    while (!is_finished()) {
        think(i, print);
        take_forks(i, print);
        eat(i, print);
        put_forks(i, print);

        if (!untagged && stats[i].n_eaten_meals == constr.n_meals_per_philo) {
            tag_finished_eating(i);
            untagged = true;
        }
    }
}

void run_problem(Constraints constraints) {
    state = std::vector<PhiloState>(
        constraints.n_philo, PhiloState::HUNGRY);
    stats = std::vector<Stats>(
        constraints.n_philo, Stats{});
    flag_all_finished = std::vector<bool>(
        constraints.n_philo, false);
    
    sync_print print;
    std::vector<std::thread> threads;
    threads.reserve(constraints.n_philo);
    cv.reserve(constraints.n_philo);

    for (auto i = 0u; i < constraints.n_philo; i++) {
        cv.push_back(std::make_unique<std::condition_variable>());
        threads.emplace_back(philosopher, i, constraints, std::ref(print));
    }

    for (auto& t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }
    
    auto view = stats | std::views::transform([](const Stats& s){
            return s.n_eaten_meals;
        });
    auto total = std::accumulate(view.begin(), view.end(), 0);
    auto [min_it, max_it] = std::minmax_element(view.begin(), view.end());
    auto avg_meal = static_cast<float>(total) / constraints.n_philo;
    auto [min, max] = std::make_pair(*min_it, *max_it);
    
    std::cout << "\n---------- Results ----------\n";
    std::cout << std::format("Total eaten meals: {}\n", total);
    std::cout << std::format("Average meals per philo: {:.1f}\n", avg_meal);
    std::cout << std::format("Min-max eaten meals: min={} max={}\n", min, max);
    
    for (auto i = 0; i < stats.size(); i++) {
        auto& s = stats[i];
        std::cout << std::format("\t[{}] meals eaten: {}\n", i, s.n_eaten_meals);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << std::format("Usage: {} <n_philosophers> <min_meals_per_philo>\n", argv[0]);
        std::cerr << std::format("Example: {} 5 30\n", argv[0]);
        return -1;
    }

    auto n_philo = std::atoi(argv[1]);
    auto n_meals = std::stoi(argv[2]);
    
    if (n_philo < 0 || n_meals < 0) {
        std::cerr << "n_philosophers and min_meals_per_philo must be non-negative integers\n";
        return -1;
    }

    run_problem(Constraints {
        static_cast<std::uint32_t>(n_meals),
        static_cast<std::uint32_t>(n_philo),
    });
}