#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <compare>
#include <iostream>
#include <format>

// Ring modulo N
template <std::size_t N>
class Ring {
    public:
    Ring() : value(0) {}
    explicit Ring(std::size_t val) : value(val % N) {};
    Ring(const Ring&) = default;
    Ring& operator=(const Ring&) = default;
    Ring(Ring&&) = default;
    Ring& operator=(Ring&&) = default;
    ~Ring() = default;

    Ring operator+(const Ring& other) const {
        return Ring((value + other.value) % N);
    }

    Ring operator-(const Ring& other) const {
        return Ring((value + N - other.value) % N);
    }

    Ring operator*(const Ring& other) const {
        return Ring((value * other.value) % N);
    }

    Ring operator/(const Ring& other) const noexcept(false) {
        if (other.value == 0) {
            throw std::invalid_argument("Division by zero in Ring");
        }
        // Find the multiplicative inverse of other.value modulo N
        auto inverse = find_inverse(other.value);
        if (inverse == 0) {
            throw std::invalid_argument("No multiplicative inverse exists in Ring");
        }
        return Ring((value * inverse) % N);
    }

    Ring& operator+=(const Ring& other) {
        value = (value + other.value) % N;
        return *this;
    }

    Ring& operator-=(const Ring& other) {
        value = (value + N - other.value) % N;
        return *this;
    }

    Ring& operator*=(const Ring& other) {
        value = (value * other.value) % N;
        return *this;
    }

    Ring& operator/=(const Ring& other) noexcept(false) {
        if (other.value == 0) {
            throw std::invalid_argument("Division by zero in Ring");
        }
        auto inverse = find_inverse(other.value);
        if (inverse == 0) {
            throw std::invalid_argument("No multiplicative inverse exists in Ring");
        }
        value = (value * inverse) % N;
        return *this;
    }

    bool operator==(const Ring& other) const {
        return value == other.value;
    }

    bool operator!=(const Ring& other) const {
        return value != other.value;
    }

    std::strong_ordering operator<=>(const Ring& other) const {
        return value <=> other.value;
    }

    std::ostream& operator<<(std::ostream& os) const {
        return os << value;
    }

    Ring inverse() const noexcept(false) {
        auto inv = find_inverse(value);
        if (inv == 0) {
            throw std::invalid_argument("No multiplicative inverse exists in Ring");
        }
        return Ring(inv);
    }

    operator std::size_t() const {
        return value;
    }

private:

    constexpr std::size_t find_inverse(std::size_t val) const {
        for (std::size_t i = 1; i < N; ++i) {
            if ((val * i) % N == 1) {
                return i;
            }
        }
        return 0; // No inverse found
    }

    std::size_t value;
};

template <std::size_t N>
struct std::formatter<Ring<N>> {
    constexpr auto parse(std::format_parse_context& ctx) {
        return ctx.begin();
    }

    auto format(const Ring<N>& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", std::size_t(obj));
    }
};