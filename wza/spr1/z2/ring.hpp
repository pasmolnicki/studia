#pragma once

#include <vector>
#include <concepts>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <array>

// Koncept Ciała (Field) (Zadanie 2)
template<typename T>
concept Field = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
    { a - b } -> std::same_as<T>;
    { a * b } -> std::same_as<T>;
    { a / b } -> std::same_as<T>;
    { T(0) } -> std::convertible_to<T>;
    { T(1) } -> std::convertible_to<T>;
};

// Klasa reprezentująca wielomian jednej zmiennej
// Reprezentacja: wektor współczynników, gdzie coefs[i] to współczynnik przy x^i.
// Dlaczego taka reprezentacja? Wektor (std::vector) pozwala na łatwe operacje na stopniach (indeksach),
// a pozbywanie się wiodących zer upraszcza wyliczanie stopnia wielomianu.
// Taka budowa pozwala łatwo rozszerzyć na wielomiany wielu zmiennych poprzez Polynomial<Polynomial<T>>.
template<Field T>
class Polynomial {
public:
    std::vector<T> coefs;

    Polynomial() = default;
    Polynomial(T scalar) : coefs{scalar} { trim(); }
    Polynomial(std::vector<T> c) : coefs(std::move(c)) { trim(); }

    void trim() {
        while (coefs.size() > 0 && coefs.back() == T(0)) {
            coefs.pop_back();
        }
    }

    bool is_zero() const {
        return coefs.empty();
    }

    // Norma zdefiniowana jako stopień wielomianu (zgodnie z def. normy euklidesowej wielomianów),
    // Przyjmujemy -1 dla wielomianu zerowego
    int norm() const {
        return is_zero() ? -1 : static_cast<int>(coefs.size()) - 1;
    }

    T leading_coef() const {
        return is_zero() ? T(0) : coefs.back();
    }

    Polynomial operator+(const Polynomial& other) const {
        Polynomial result;
        size_t max_deg = std::max(coefs.size(), other.coefs.size());
        result.coefs.resize(max_deg, T(0));
        for (size_t i = 0; i < max_deg; ++i) {
            T a = i < coefs.size() ? coefs[i] : T(0);
            T b = i < other.coefs.size() ? other.coefs[i] : T(0);
            result.coefs[i] = a + b;
        }
        result.trim();
        return result;
    }

    Polynomial operator-(const Polynomial& other) const {
        Polynomial result;
        size_t max_deg = std::max(coefs.size(), other.coefs.size());
        result.coefs.resize(max_deg, T(0));
        for (size_t i = 0; i < max_deg; ++i) {
            T a = i < coefs.size() ? coefs[i] : T(0);
            T b = i < other.coefs.size() ? other.coefs[i] : T(0);
            result.coefs[i] = a - b;
        }
        result.trim();
        return result;
    }

    Polynomial operator*(const Polynomial& other) const {
        if (is_zero() || other.is_zero()) return Polynomial();
        std::vector<T> res_coefs(coefs.size() + other.coefs.size() - 1, T(0));
        for (size_t i = 0; i < coefs.size(); ++i) {
            for (size_t j = 0; j < other.coefs.size(); ++j) {
                res_coefs[i + j] = res_coefs[i + j] + (coefs[i] * other.coefs[j]);
            }
        }
        return Polynomial(res_coefs);
    }

    // Zwraca parę (Q, R): Wynik dzielenia i resztę
    std::pair<Polynomial, Polynomial> div_mod(const Polynomial& divisor) const {
        if (divisor.is_zero()) {
            throw std::invalid_argument("Division by zero polynomial");
        }
        Polynomial q;
        Polynomial r = *this;

        while (!r.is_zero() && r.norm() >= divisor.norm()) {
            int deg_diff = r.norm() - divisor.norm();
            T lc_r = r.leading_coef();
            T lc_d = divisor.leading_coef();
            T coef_diff = lc_r / lc_d;

            std::vector<T> term_coefs(deg_diff + 1, T(0));
            term_coefs[deg_diff] = coef_diff;
            Polynomial term(term_coefs);

            q = q + term;
            r = r - (term * divisor);
        }

        return {q, r};
    }

    Polynomial operator/(const Polynomial& other) const {
        return div_mod(other).first;
    }

    Polynomial operator%(const Polynomial& other) const {
        return div_mod(other).second;
    }

    Polynomial make_monic() const {
        if (is_zero()) return *this;
        T lc = leading_coef();
        std::vector<T> new_coefs = coefs;
        for (auto& c : new_coefs) {
            c = c / lc;
        }
        return Polynomial(new_coefs);
    }
};

template <Field T>
Polynomial<T> gcd(Polynomial<T> a, Polynomial<T> b) {
    while (!b.is_zero()) {
        auto r = a % b;
        a = b;
        b = r;
    }
    return a.make_monic();
}

template <Field T>
Polynomial<T> lcm(Polynomial<T> a, Polynomial<T> b) {
    if (a.is_zero() && b.is_zero()) return Polynomial<T>();
    Polynomial<T> p = (a * b) / gcd(a, b);
    return p.make_monic();
}

// Rozszerzony algorytm Euklidesa: zwraca (gcd, x, y) takie że a*x + b*y = gcd
template <Field T>
std::tuple<Polynomial<T>, Polynomial<T>, Polynomial<T>> extended_gcd(Polynomial<T> a, Polynomial<T> b) {
    Polynomial<T> old_r = a, r = b;
    Polynomial<T> old_s = Polynomial<T>(T(1)), s = Polynomial<T>(T(0));
    Polynomial<T> old_t = Polynomial<T>(T(0)), t = Polynomial<T>(T(1));

    while (!r.is_zero()) {
        auto [q, rem] = old_r.div_mod(r);
        old_r = r;
        r = rem;

        Polynomial<T> temp_s = old_s - q * s;
        old_s = s;
        s = temp_s;

        Polynomial<T> temp_t = old_t - q * t;
        old_t = t;
        t = temp_t;
    }
    
    // Normalizacja do monicznego
    T lc = old_r.leading_coef();
    if (lc != T(0)) {
        Polynomial<T> norm(std::vector<T>{T(1)/lc});
        old_r = old_r * norm;
        old_s = old_s * norm;
        old_t = old_t * norm;
    }
    return {old_r, old_s, old_t};
}

template<Field T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& p) {
    if (p.is_zero()) return out << "0";
    bool first = true;
    for (int i = p.norm(); i >= 0; --i) {
        if (p.coefs[i] == T(0)) continue;
        if (!first && p.coefs[i] > T(0)) out << " + ";
        else if (p.coefs[i] < T(0)) {
            out << (first ? "-" : " - ");
        }
        T abs_c = p.coefs[i] < T(0) ? T(0) - p.coefs[i] : p.coefs[i];
        if (abs_c != T(1) || i == 0) out << abs_c;
        if (i > 0) out << "x";
        if (i > 1) out << "^" << i;
        first = false;
    }
    return out;
}

// Zadanie 3: Porządki produktowe
template <size_t N>
using Tuple = std::array<unsigned int, N>;

template <size_t N>
bool operator<=(const Tuple<N>& lhs, const Tuple<N>& rhs) {
    for (size_t i = 0; i < N; ++i) {
        if (lhs[i] > rhs[i]) return false;
    }
    return true;
}

template <size_t N>
std::vector<Tuple<N>> find_minimal_elements(const std::vector<Tuple<N>>& A) {
    std::vector<Tuple<N>> minimal;
    for (size_t i = 0; i < A.size(); ++i) {
        bool is_min = true;
        for (size_t j = 0; j < A.size(); ++j) {
            if (i == j) continue;
            if (A[j] <= A[i] && !(A[i] <= A[j])) {
                is_min = false;
                break;
            }
        }
        if (is_min) {
            // Unikanie duplikatów: element już znajduje się w minimal
            bool exists = false;
            for(const auto& m : minimal) {
                bool eq = true;
                for(size_t k = 0; k < N; ++k) {
                    if(m[k] != A[i][k]) eq = false;
                }
                if(eq) exists = true;
            }
            if(!exists) minimal.push_back(A[i]);
        }
    }
    return minimal;
}
