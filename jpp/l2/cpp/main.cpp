#include <iostream>
#include <format>
#include <string>
#include <stdexcept>
#include "ring.hpp"

// Simple testing framework
int tests_run = 0;
int tests_passed = 0;

void print_result(const std::string& test_name, bool passed) {
    tests_run++;
    if (passed) {
        tests_passed++;
        std::cout << std::format("\033[32m[ PASS ]\033[0m {}\n", test_name);
    } else {
        std::cout << std::format("\033[31m[ FAIL ]\033[0m {}\n", test_name);
    }
}

template <typename T, typename U>
void test_eq(const std::string& test_name, const T& actual, const U& expected) {
    if (actual == expected) {
        print_result(test_name, true);
    } else {
        print_result(test_name, false);
        std::cout << std::format("         Expected: {}, Actual: {}\n", expected, actual);
    }
}

template <typename Func>
void test_throws(const std::string& test_name, Func f) {
    try {
        f();
        print_result(test_name, false);
        std::cout << "         Expected an exception to be thrown.\n";
    } catch (const std::invalid_argument&) {
        print_result(test_name, true);
    } catch (...) {
        print_result(test_name, false);
        std::cout << "         Expected std::invalid_argument, but got a different exception.\n";
    }
}

void test_constructors() {
    std::cout << "\n--- Testing Constructors & Casts ---\n";
    Ring<5> a;
    test_eq("Default constructor initializes to 0", static_cast<std::size_t>(a), 0);
    
    Ring<5> b(7);
    test_eq("Value constructor applies modulo", static_cast<std::size_t>(b), 2);
    
    Ring<5> c(b);
    test_eq("Copy constructor copies value", static_cast<std::size_t>(c), 2);
}

void test_addition() {
    std::cout << "\n--- Testing Addition ---\n";
    Ring<7> a(3), b(5);
    test_eq("3 + 5 mod 7 == 1", static_cast<std::size_t>(a + b), 1);
    
    Ring<7> c(3); c += Ring<7>(5);
    test_eq("+= operator works correctly", static_cast<std::size_t>(c), 1);
}

void test_subtraction() {
    std::cout << "\n--- Testing Subtraction ---\n";
    Ring<7> a(2), b(5);
    test_eq("2 - 5 mod 7 == 4", static_cast<std::size_t>(a - b), 4);
    
    Ring<7> c(2); c -= Ring<7>(5);
    test_eq("-= operator works correctly", static_cast<std::size_t>(c), 4);
    
    Ring<7> d(5), e(2);
    test_eq("5 - 2 mod 7 == 3", static_cast<std::size_t>(d - e), 3);
}

void test_multiplication() {
    std::cout << "\n--- Testing Multiplication ---\n";
    Ring<7> a(3), b(4);
    test_eq("3 * 4 mod 7 == 5", static_cast<std::size_t>(a * b), 5);
    
    Ring<7> c(3); c *= Ring<7>(4);
    test_eq("*= operator works correctly", static_cast<std::size_t>(c), 5);
}

void test_division_and_inverse() {
    std::cout << "\n--- Testing Division & Inverse ---\n";
    
    // Valid inverse (gcd(val, N) == 1)
    Ring<7> a(3);
    test_eq("Inverse of 3 mod 7 is 5", static_cast<std::size_t>(a.inverse()), 5);
    
    Ring<7> b(2), c(3);
    test_eq("2 / 3 mod 7 == 2 * 5 mod 7 == 3", static_cast<std::size_t>(b / c), 3);
    
    Ring<7> d(2); d /= Ring<7>(3);
    test_eq("/= operator works correctly", static_cast<std::size_t>(d), 3);

    // Exceptions
    Ring<7> e(5), zero(0);
    test_throws("Division by zero throws", [&]() { e / zero; });
    test_throws("/= by zero throws", [&]() { e /= zero; });
    test_throws("Inverse of 0 throws", [&]() { zero.inverse(); });

    Ring<6> f(2); // 2 has no inverse in Z_6 since gcd(2, 6) != 1
    test_throws("Inverse without multiplicative inverse throws", [&]() { f.inverse(); });
    test_throws("Division without multiplicative inverse throws", [&]() { Ring<6>(5) / f; });
}

void test_comparisons() {
    std::cout << "\n--- Testing Comparisons ---\n";
    Ring<5> a(2), b(7), c(3);
    
    test_eq("2 == 7 mod 5", a == b, true);
    test_eq("2 != 3 mod 5", a != c, true);
    test_eq("2 < 3 mod 5", a < c, true);
    test_eq("3 > 2 mod 5", c > a, true);
    test_eq("2 == 7 mod 5", a == b, true);
}

int main() {
    std::cout << "Starting Ring<N> Tests...\n";
    
    test_constructors();
    test_addition();
    test_subtraction();
    test_multiplication();
    test_division_and_inverse();
    test_comparisons();

    std::cout << std::format("\n==================================\n");
    std::cout << std::format("Tests Run: {}, Passed: {}, Failed: {}\n", tests_run, tests_passed, tests_run - tests_passed);
    std::cout << std::format("==================================\n");

    return tests_run == tests_passed ? 0 : 1;
}