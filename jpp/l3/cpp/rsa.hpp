#pragma once

#include "ring.hpp"
#include <random>

template<std::size_t P, std::size_t Q>
class RSA {
    constexpr static auto N = P * Q;
    
    constexpr std::size_t gcd(std::size_t a, std::size_t b) {
        while(b != 0) {
            auto t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    constexpr std::size_t get_lambda() {
        return N / gcd(P, Q);
    }

    constexpr std::size_t find_exponent(std::size_t lambda) {
        // Find such e, so that gcd(e, lambda) == 1
        auto e = (1uz << 16) + 1;
        std::mt19937_64 engine;
        std::uniform_int_distribution<std::size_t> dist(e);

        while(gcd(e, lambda) != 1) {
            e = dist(engine);
        }
    }

    std::size_t exp{0}, lambda{0}, d{0};

public:
    constexpr RSA() {
        // Calc private key
        constexpr auto L = get_lambda();
        lambda = L;
        exp = find_exponent(lambda);
        d = Ring<L>(e).inverse();
    }

    std::size_t getPublicKey() {
        return e;
    }
};
