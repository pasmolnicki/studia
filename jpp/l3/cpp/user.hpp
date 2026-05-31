#pragma once

#include "dhsetup.hpp"

template<std::size_t P>
class User {

    std::size_t gen, secret, public_key, secret_key;
public:
    User(DHSetup<P>& dh) {
        this->gen = dh.getGenerator();
        auto engine = std::mt19937_64(std::random_device{}());
        this->secret = std::uniform_int_distribution<std::size_t>(2, P)(engine);
        this->public_key = Ring<P>(gen).pow(secret);
    }

    std::size_t getPublicKey() {
        return this->public_key;
    }

    void setKey(std::size_t key) {
        this->secret_key = Ring<P>(key).pow(secret);
    }

    std::size_t encrypt(std::size_t msg) {
        return Ring<P>(msg) * Ring<P>(secret_key);
    }

    std::size_t decrypt(std::size_t cipher) {
        return Ring<P>(cipher) / Ring<P>(secret_key);
    }
};