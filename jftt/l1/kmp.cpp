#include <cstddef>
#include <print>
#include <string_view>
#include <vector>
#include "common.hpp"
#include <string.h>

using state_t = int;

std::vector<state_t> compute_prefix_fn(std::string_view pattern)
{
    std::vector<state_t> prefix(pattern.size(), -1);
    auto k = -1;
    for (auto q{1}; q < pattern.size(); q++) {
        while (k > 0 && pattern[k + 1] != pattern[q]) {
            k = prefix[k];
        }

        if (pattern[k + 1] == pattern[q]) {
            k = k + 1;
        }

        prefix[q] = k;
    }

    return prefix;
}


std::vector<std::size_t> kmp_matcher(std::string text, std::string_view pattern)
{
    std::vector<std::size_t> matches;
    auto prefix = compute_prefix_fn(pattern);

    auto q = -1;
    for (auto i{0}; i < text.size(); i++) {
        while (q >= 0 && pattern[q + 1] != text[i]) {
            q = prefix[q];
        }

        if (pattern[q + 1] == text[i]) {
            q = q + 1;
        }
        if (q == pattern.size() - 1) {
            matches.push_back(i - pattern.size() + 1);
            q = prefix[q];
        }
    }

    return matches;
}


int main(int argc, char** argv)
{
    auto opts = parse_args(argc, temp);
    auto text = get_text(opts);

    auto matches = kmp_matcher(text, opts.pattern);

    for (auto m : matches) {
        std::println("match at {}", m);
    }

    return 0;
}
