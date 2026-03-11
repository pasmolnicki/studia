#include <iostream>
#include <memory>
#include <format>
#include <concepts>
#include <optional>
#include <vector>
#include <algorithm>

/*
Zadanie 1. [14 p.]
Zaimplementuj kolejkę (FIFO first in first out) oraz stos (LIFO last in first out). Do każdej z
nich dodaj po 50 elementów, a następnie pobierz je z kolejek (wypisz po kolei elementy
dodawane oraz wyjmowane). Pamiętaj o obsłudze błędów (próba pobrania elementu z
pustej kolejki).
*/


template <std::copyable T>
class base_queue {
public:
    using value = T;
    using node = struct Node {
        value val{};
        std::shared_ptr<struct Node> next{nullptr};

        Node(const value& v) : val(v), next(nullptr) {}
    };
    using node_ptr = std::shared_ptr<node>;

protected:
    std::size_t M_size{0};
    node_ptr M_head{nullptr};
    node_ptr M_tail{nullptr};
public:
    base_queue() = default;

    virtual void push(const value& v) noexcept = 0;    

    // Pop results in receiving the first value of the queue (head)
    std::optional<value> pop() noexcept {
        if (!this->M_head) {
            return std::nullopt;
        }

        this->M_size--;
        auto v = this->M_head->val;
        this->M_head = this->M_head->next;
        return {v};
    }

    constexpr std::size_t size() const noexcept {
        return M_size;
    }

    constexpr bool empty() const noexcept {
        return M_head.get() == nullptr;
    }
};

template <std::copyable T>
class fifo_queue : public base_queue<T> {
public:
    using value = base_queue<T>::value;
    using node = base_queue<T>::node;

    using base_queue<T>::size;
    using base_queue<T>::empty;
    using base_queue<T>::pop;

    fifo_queue() = default;

    // Push results in appending new value at the END of the
    // queue, (FIFO - first in first out, so order of appened elements 
    // is preserved)
    void push(const value& v) noexcept override {
        this->M_size++;

        if (!this->M_tail && !this->M_head) {
            this->M_head = std::make_shared<node>(v);
            this->M_tail = this->M_head;
            return;
        }

        // Else just append at the end
        this->M_tail->next = std::make_shared<node>(v);
        this->M_tail = this->M_tail->next;
    }
};

template <std::copyable T>
class lifo_queue : public base_queue<T> {
public:
    using value = base_queue<T>::value;
    using node = base_queue<T>::node;

    using base_queue<T>::size;
    using base_queue<T>::empty;
    using base_queue<T>::pop;

    lifo_queue() = default;

    // Push results in appending new value at the BEGINNING of the queue
    void push(const value& v) noexcept override {
        this->M_size++;

        if (!this->M_tail && !this->M_head) {
            this->M_head = std::make_shared<node>(v);
            this->M_tail = this->M_head;
            return;
        }

        // Else just append at the start
        auto new_node = std::make_shared<node>(v);
        new_node->next = this->M_head;
        this->M_head = new_node;
    }
};

template <std::copyable T>
void print_queue(base_queue<T>& q) {
    while (true) {
        auto p = q.pop();
        if (p.has_value()) {
            std::cout << p.value() << '\n';
        } else {
            break;
        }
    }
}

int main() {
    // struct ex {
    //     int a;
    //     constexpr ex& operator=(const ex& e) = delete;
    // };

    fifo_queue<int> fifo;
    lifo_queue<int> lifo;

    // std::vector<ex> items{ex{0}, ex{1}, ex{2}};
    std::vector<int> items(50, 0);
    int i = 1;
    std::generate(items.begin(), items.end(), [&i](){
        return i++;
    });

    std::cout << "Elementy: \n";

    for (auto i : items) {
        lifo.push(i);
        fifo.push(i);

        std::cout << i << '\n';
    }

    std::cout << "\n==========\n";
    std::cout << "FIFO:\n";
    print_queue(fifo);

    std::cout << "\n==========\n";
    std::cout << "LIFO:\n";
    print_queue(lifo);

    // Nic się nie dzieje
    fifo.pop();
    lifo.pop();

    // Nie powienień mieć żadnej wartości
    if (fifo.pop().has_value() || !fifo.empty()) {
        std::cout << __FILE__ << ':' << __LINE__ << " FAILED ASSERT\n";
    }
}
