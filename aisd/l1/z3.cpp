#include <iostream>
#include <memory>
#include <format>
#include <concepts>
#include <vector>
#include <algorithm>
#include <random>
#include <format>

/*
Zadanie 3. [15 p.]
    Zaimplementuj listę dwukierunkową cykliczną i wykonaj te same polecenia co w
    poprzednim zadaniu. W tym przypadku, każdy element ma dodatkowo wskaźnik na
    poprzedni element w cyklu. Przy badaniu kosztów losowych wyszukiwań, każde
    wyszukiwanie na początku losowo decyduje w którym kierunku będzie się odbywać.
*/

template <std::copyable T>
class circular_linked_list {
public:
    using value = T;
    using node = struct Node {
        value val{};
        std::shared_ptr<Node> next{nullptr};
        std::shared_ptr<Node> prev{nullptr};

        Node(const value& v) : val(v), next(nullptr), prev(nullptr) {}
        ~Node() { prev = nullptr; }
    };
    using node_ptr = std::shared_ptr<node>;
    using raw_node_ptr = node*;

private:
    node_ptr M_head{nullptr};
    std::size_t M_size{0};

    node_ptr M_find_tail() {
        auto ptr = M_head;
        while (ptr->next != M_head) {
            ptr = ptr->next;
        }
        return ptr;
    }

public:

    template <typename V>
    class list_iterator {
        raw_node_ptr M_ptr;
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = V;
        using pointer = raw_node_ptr;
        using reference = V&;

        list_iterator() = default;
        list_iterator(pointer ptr): M_ptr(ptr) {}
        list_iterator(const list_iterator& other) { M_ptr = other.M_ptr; }
        list_iterator(list_iterator&& other) {M_ptr = std::move(other.M_ptr); }

        list_iterator& operator=(const list_iterator& other) { M_ptr = other.M_ptr; return *this; }
        list_iterator& operator=(list_iterator&& other) { M_ptr = std::move(other.M_ptr); return *this; }

        list_iterator& operator++() { M_ptr = M_ptr->next.get(); return *this; }
        list_iterator& operator++(int) { auto v = M_ptr; M_ptr = M_ptr->next.get(); return {v}; }
        list_iterator& operator--() { M_ptr = M_ptr->prev.get(); return *this; }
        list_iterator& operator--(int) { auto v = M_ptr; M_ptr = M_ptr->prev.get(); return {v}; }

        bool operator==(const list_iterator& other) const { return M_ptr == other.M_ptr; }
        reference operator*() { return M_ptr->val; }
    };

    typedef list_iterator<T> iterator;
    typedef list_iterator<const T> const_iterator;

    circular_linked_list() = default;
    circular_linked_list(const std::vector<T>& elems) {
        for (const auto& t : elems) {
            this->push(t);
        }
    }
    ~circular_linked_list() {
        if (M_head) {
            M_find_tail()->next = nullptr;
        }
    }

    void push(const value& elem) noexcept {
        if (!M_head) {
            M_head = std::make_shared<node>(elem);
            M_head->next = M_head;
            M_head->prev = M_head;
            return;
        }

        auto p = M_head->next;
        M_head->next = std::make_shared<node>(elem);
        M_head->next->next = p;

        // auto ptr = M_find_tail();
        // ptr->next = std::make_shared<node>(elem);
        // ptr->next->next = M_head;
    }

    void append(const circular_linked_list& other) noexcept {
        auto it = other.begin();
        while (true) {
            this->push(*it);
            if (++it == other.begin()) {
                break;
            }
        }
    }

    iterator begin() {
        return iterator(M_head.get());
    }

    const_iterator begin() const {
        return const_iterator(M_head.get());
    }
};

constexpr int MAX_NUMBER = 1e5;
constexpr int N_ELEMS = 1e4;
constexpr int N_LOOKUPS = 1e3;

std::vector<int> generate_randoms(std::size_t seed) {
    std::vector<int> v(N_ELEMS, 0);
    std::uniform_int_distribution<int> dist(0, MAX_NUMBER);
    std::mt19937 gen(seed);

    std::generate(v.begin(), v.end(), [&dist, &gen](){ return dist(gen); });
    return v;
}

void run_experiment(bool in_list = true) {
    const auto max_dist = in_list ? N_ELEMS : MAX_NUMBER;
    std::random_device rd{};
    std::uniform_int_distribution<int> dist{0, max_dist};
    std::mt19937 gen{rd()};
    std::bernoulli_distribution dir_dist(0.5);

    auto nums = generate_randoms(rd());
    circular_linked_list<int> list{nums};

    auto total_tries = 0ULL;
    for (int i = 0; i < N_LOOKUPS; i++) {
        int num_to_find = dist(gen);

        if (in_list) {
            num_to_find = nums[num_to_find];
        }

        // Find it
        bool go_forward = dir_dist(gen) >= 0.5;
        auto it = list.begin();
        while (true) {
            total_tries++;
            auto v = *it;

            if (v == num_to_find) {
                break;
            }
            
            if (go_forward) {
                ++it;
            } else {
                --it;
            }

            // End of search - made a cycle
            if (it == list.begin()) {
                break;
            }
        }
    }

    float avg = total_tries / static_cast<float>(N_ELEMS);
    std::cout << std::format("Average iterations {}: {:.2f}\n", in_list ? "IN-LIST" : "RAND-NUM", avg);
}


int main() {
    // run_experiment(true);
    // run_experiment(false);
    circular_linked_list<int> foo{{1, 2, 3, 5}};
}