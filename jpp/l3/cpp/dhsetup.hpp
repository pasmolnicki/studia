#pragma once

#include <random>
#include <cstddef>

#include "ring.hpp"

template <std::size_t P>
class DHSetup {

    static std::vector<std::size_t> factorize() {
        std::vector<std::size_t> a;
        std::size_t n = P - 1;
        std::size_t div = 2;
        bool first_div = true;

        while (n != 1) {
            if (n % div == 0) {
                if (first_div) {
                    a.push_back(div);
                    first_div = false;
                }
                n /= div;
            } else {
                div++;
                first_div = true;
            }
        }

        return a;
    }

    static bool is_generator(const std::vector<std::size_t>& factors, std::size_t n) {
        for (auto& f : factors) {
            if (Ring<P>(n).pow((P - 1) / f) == 1) {
                return false;
            }
        }
        return true;
    }

    inline std::size_t find_generator() {
        std::mt19937_64 engine(std::random_device{}());
        std::uniform_int_distribution<std::size_t> dist{1, P};
        auto factors = factorize();

        std::size_t g = dist(engine);
        while (!is_generator(factors, g)) {
            g = dist(engine);
        }

        return g;
    }

    std::size_t gen;

public:
    DHSetup() {
        gen = find_generator();
    }

    std::size_t getGenerator() {
        return gen;
    }

    std::size_t power(std::size_t a, std::size_t b) {
        return Ring<P>(a).pow(b);
    }
};

/*
import random
P = 10007

def factorize(n):
    a = []
    div = 2
    first = True
    while n != 1:
            if n % div == 0:
                    if first:
                            a.append(div)
                            first = False
                    n /= div
            else:
                    div += 1
                    first = True
    return a

def is_generator(factors, g):
    for f in factors:
        if pow(g, (P-1) // f, P) == 1:
            return False
    return True

def find_generator():
    factors = factorize(P-1)
    g = random.randint(1, P-1)
    while not is_generator(factors, g):
        g = random.randint(1, P-1)
    return g
*/