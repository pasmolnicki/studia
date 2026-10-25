#include <algorithm>
#include <cctype>
#include <print>
#include <vector>
#include <functional>

#include "common.hpp"

using state_t = int;
using transition_t = std::vector<std::vector<state_t>>;
using indexing_ft = std::function<std::size_t(char)>;

state_t gen_tf_next_state(std::string_view pattern, state_t q, char c)
{
    // This char matches the pattern
    if (q < pattern.size() && c == pattern[q]) {
        return q + 1;
    }

    for (int ns{q}; ns > 0; ns--)
    {
        if (pattern[ns - 1] == c)
        {
            int i{0};
            for (; i < ns - 1; i++) {
                if (pattern[i] != pattern[q - ns + 1 + i]) {
                    break;
                }
            }
            if (i == ns - 1) {
                return ns;
            }
        }
    }

    return 0;
}

transition_t gen_tf(std::string_view pattern, std::string_view alphabet, indexing_ft index_mapper) {
    const auto m = pattern.size();
    const auto n = alphabet.size();
    transition_t tf(m + 1, std::vector<state_t>(n, 0));

    for (state_t q = 0; q <= m; q++) {
        std::for_each(alphabet.begin(), alphabet.end(), [&tf, &index_mapper, q, &pattern, m](char c){
            auto k = index_mapper(c);
            tf[q][k] = gen_tf_next_state(pattern, q, c);
        });
    }

    return tf;
}

std::vector<std::size_t> finite_automata_matcher(std::string text, const transition_t& t, std::size_t m, indexing_ft index_mapper)
{
    std::vector<std::size_t> matches;
    state_t q = 0;
    for (std::size_t i{0}; i < text.size(); i++) {
        if (not isalpha(static_cast<unsigned char>(text[i]))) {
            continue;
        }

        q = t.at(q).at(index_mapper(text[i]));
        if (q == m) {
            matches.push_back(i - m + 1);
        }
    }
    return matches;
}

int main(int argc, char** argv)
{
    constexpr auto alphabet = "abcdefghijklmnoprstuvwxyz";
    constexpr auto index_mapper = [](char c) { return c - 'a'; };

    auto opts = parse_args(argc, argv);
    auto text = get_text(opts);

    auto tf = gen_tf(opts.pattern, alphabet, index_mapper);
    auto matches = finite_automata_matcher(text, tf, opts.pattern.size(), index_mapper);

    for (auto m : matches) {
        std::println("match at {}", m);
    }

    return 0;
}
