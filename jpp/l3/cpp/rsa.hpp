#pragma once

#include <random>

#include "ring.hpp"

template<std::size_t P, std::size_t Q>
class RSA {
    constexpr static auto N = P * Q;
    
    static constexpr std::size_t gcd(std::size_t a, std::size_t b) {
        while(b != 0) {
            auto t = b;
            b = a % b;
            a = t;
        }
        return a;
    }

    static constexpr std::size_t get_lcm() {
        return (P-1)*(Q-1) / gcd(P-1, Q-1);
    }

    inline std::size_t find_exponent(std::size_t lambda) {
        std::mt19937_64 engine(std::random_device{}());
        std::uniform_int_distribution<std::size_t> dist(2, lambda - 1);

        auto e = dist(engine);
        while(gcd(e, lambda) != 1) {
            e = dist(engine);
        }

        return e;
    }

    std::size_t exp{0}, lcm{0}, d{0};

public:
    constexpr RSA() {
        // Calc private key
        constexpr auto L = get_lcm();
        lcm = L;
        exp = find_exponent(lcm);
        d = Ring<L>(exp).inverse();
    }

    std::size_t getPublicKey() {
        return exp;
    }

    std::size_t encrypt(std::size_t message, std::size_t public_key) {
        return Ring<N>(message).pow(public_key);

        // return fast_pow(message, public_key, N);
    }

    std::size_t decrypt(std::size_t cipher) {
        return Ring<N>(cipher).pow(d);
        // return fast_pow(cipher, d, N);
    }

    constexpr unsigned int getModulo() const {
        return N;
    }
};
