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
        return Ring((value * guard_inverse(other.value)) % N);
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
        value = (value * guard_inverse(other.value)) % N;
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

    bool operator<(const Ring& other) const {
        return ((*this) <=> other) < 0;
    }

    bool operator>(const Ring& other) const {
        return ((*this) <=> other) > 0;
    }

    bool operator<=(const Ring& other) const {
        return this->value <= other.value;
    }

    bool operator>=(const Ring& other) const {
        return this->value >= other.value;
    }

    std::ostream& operator<<(std::ostream& os) const {
        return os << value;
    }

    Ring inverse() const  {
        return Ring(find_inverse(this->value));
    }

    // template<std::size_t P>
    // constexpr Ring<P> inverse() const {
    //     return Ring<P>(find_inverse(this->value, P));
    // }

    operator std::size_t() const {
        return value;
    }

private:
    // Find the multiplicative inverse of other.value modulo N, it may throw
    std::size_t guard_inverse(std::size_t val) const noexcept(false) {
        auto inverse = find_inverse(val);
        if (inverse == 0) {
            throw std::invalid_argument("No multiplicative inverse exists in Ring");
        }
        return inverse;
    }

    /*
    https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Modular_integers
    function inverse(a, n)
        t := 0;     newt := 1
        r := n;     newr := a

        while newr ≠ 0 do
            quotient := r div newr
            (t, newt) := (newt, t − quotient × newt) 
            (r, newr) := (newr, r − quotient × newr)

        if r > 1 then
            return "a is not invertible"
        if t < 0 then
            t := t + n

        return t
    */

    constexpr std::size_t find_inverse(std::size_t val, std::size_t n = N) const {
        using signed_t = long long;

        signed_t t = 0;
        signed_t new_t = 1;
        signed_t r = n;
        signed_t new_r = val % n;

        while (new_r != 0) {
            signed_t q = r / new_r;

            signed_t tmp_t = t - q * new_t;
            t = new_t;
            new_t = tmp_t;

            signed_t tmp_r = r - q * new_r;
            r = new_r;
            new_r = tmp_r;
        }

        if (r > 1) {
            return 0;
        }

        if (t < 0) {
            t += N;
        }

        return t;
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