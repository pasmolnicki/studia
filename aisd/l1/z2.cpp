#include <iostream>
#include <memory>
#include <format>
#include <concepts>
#include <vector>
#include <algorithm>
#include <random>
#include <format>

/*
Zadanie 2. [15 p.]
Zaimplementuj listę jednokierunkową cykliczną liczb całkowitych.
Lista jest dostępna przez strukturę zawierającą liczbę elementów i wskaźnik do jednego z
nich oraz liczbę elementów listy. Każdy element listy ma wskaźnik na następny element w
cyklu.

●   Zaimplementuj funkcje:
        ○ insert(l, i) wstawiającą nowy element z liczbą i do listy l,
        ○ merge(l1, l2) łączącą dwie listy l1, l2 i zademonstruj jej działanie dla
        list długości 10 zawierających dwucyfrowe liczby nieujemne.
●   Utwórz tablicę T zawierającą 10000 losowych liczb całkowitych z przedziału
    I = [0, ..., 100000], a następnie wstaw te liczby do listy L. Następnie wyznacz średni
    koszt tysiąca wyszukiwań losowych liczb z przedziału I na liście. (Przez "koszt"
    rozumiemy ilość wykonywanych porównań między elementem wyszukiwanym a
    elementami na liście.) Zbadaj dwa przypadki:
        ○ wyszukiwanie liczb, które są na liście (wybieranych losowo z tablicy T),
        ○ wyszukiwanie losowej liczby z I.
*/

template <std::copyable T>
class circular_list {
public:
    using value = T;
    using node = struct Node {
        value val{};
        std::shared_ptr<Node> next{nullptr};

        Node(const value& v) : val(v), next(nullptr) {}
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
        using iterator_category = std::forward_iterator_tag;
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

        bool operator==(const list_iterator& other) const { return M_ptr == other.M_ptr; }
        reference operator*() { return M_ptr->val; }
    };

    typedef list_iterator<T> iterator;
    typedef list_iterator<const T> const_iterator;

    circular_list() = default;
    circular_list(const std::vector<T>& elems) {
        for (const auto& t : elems) {
            this->push(t);
        }
    }
    ~circular_list() {
        if (M_head) {
            M_find_tail()->next = nullptr;
        }
    }

    void push(const value& elem) noexcept {
        M_size++;
        if (!M_head) {
            M_head = std::make_shared<node>(elem);
            M_head->next = M_head;
            return;
        }

        auto p = M_head->next;
        M_head->next = std::make_shared<node>(elem);
        M_head->next->next = p;

        // auto ptr = M_find_tail();
        // ptr->next = std::make_shared<node>(elem);
        // ptr->next->next = M_head;
    }

    void append(const circular_list& other) noexcept {
        auto it = other.begin();
        while (true) {
            this->push(*it);
            if (++it == other.begin()) {
                break;
            }
        }
    }

    std::size_t size() const {
        return M_size;
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

    auto nums = generate_randoms(rd());
    circular_list<int> list{nums};

    auto total_tries = 0ULL;
    for (int i = 0; i < N_LOOKUPS; i++) {
        int num_to_find = dist(gen);

        if (in_list) {
            num_to_find = nums[num_to_find];
        }

        // Find it
        auto it = list.begin();
        while (true) {
            total_tries++;
            auto v = *it;

            if (v == num_to_find) {
                break;
            }
            
            // End of search - made a cycle
            if (++it == list.begin()) {
                break;
            }
        }
    }

    float avg = total_tries / static_cast<float>(N_ELEMS);
    std::cout << std::format("Average iterations {}: {:.2f}\n", in_list ? "IN-LIST" : "RAND-NUM", avg);
}


int main() {
    run_experiment(true);
    run_experiment(false);
}