#include <iostream>
#include <format>

#include "rsa.hpp"
#include "diff-hell.hpp"

template<typename T>
void print_results(T& alice, T& bob, std::size_t cipher, std::size_t decrypted, std::size_t message) {
    std::cout << std::format("Bob's public key: {}\n", bob.getPublicKey());
    std::cout << std::format("Alice's public key: {}\n", alice.getPublicKey());
    std::cout << std::format("Message: {}\n", message);
    std::cout << std::format("Cipher: {}\n", cipher);
    std::cout << std::format("Decrypted: {}\n", decrypted);
    std::cout << std::format("Decryption successful: {}\n", decrypted == message ? "Yes" : "No");
}

void rsa_example() {
    std::cout << "RSA encryption example:\n";

    constexpr auto P = 10007, Q = 10009;
    RSA<P, Q> bob;
    RSA<P, Q> alice;

    std::size_t message = 12345;
    auto cipher = bob.encrypt(message, alice.getPublicKey());
    auto decrypted = alice.decrypt(cipher);

    print_results(alice, bob, cipher, decrypted, message);
}

void diff_hell() {
    std::cout << "Diffie-Hellman key exchange example:\n";

    constexpr auto P = 1234567891;
    DHSetup<P> setup;
    
    User<P> alice(setup);
    User<P> bob(setup);

    alice.setKey(bob.getPublicKey());
    bob.setKey(alice.getPublicKey());

    std::size_t message = 12345;
    auto cipher = alice.encrypt(message);
    auto decrypted = bob.decrypt(cipher);

    std::cout << std::format("Generator: {}\n", setup.getGenerator());
    print_results(alice, bob, cipher, decrypted, message);
}

int main() {
    rsa_example();
    std::cout << "\n\n";
    diff_hell();
    return 0;
}

