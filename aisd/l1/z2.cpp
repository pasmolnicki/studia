#include <iostream>
#include <memory>
#include <format>
#include <concepts>
#include <optional>
#include <vector>
#include <algorithm>

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
        std::shared_ptr<T> next{nullptr};

        Node(const value& v) : val(v), next(nullptr) {}
    };
    using node_ptr = std::shared_ptr<node>;

private:
    node_ptr M_head{nullptr};
    std::size_t M_size{0};

public:
    circular_list() = default;

    void insert(const value& elem) noexcept {
        if (!M_head) {
            M_head = std::make_shared<node>(elem);
            M_head->next = M_head;
            return;
        }

        // Find the 'tail' and set it to head
        for (auto ptr = M_head;; ptr = ptr->next) {
            if (ptr->next == M_head) {
                ptr->next = std::make_shared<node>(elem);
                ptr->next->next = M_head;
                break;
            }
        }
    }

    void merge(const circular_list& other) noexcept {
        
    }
};

int main() {

}