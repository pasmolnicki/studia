#include <print>
#include <stdexcept>

#include "common.hpp"

enum class Color {
    Null,
    White,
    Black,
};

inline Color opposite(Color c) {
    return c == Color::White ? Color::Black : Color::White;
}

struct BipartiteVisitor : public algo::BfsVisitor {
    BipartiteVisitor(std::size_t n) : m_map(n, Color::Null) {}

    void visit_edge(std::size_t u, std::size_t v) override {
        if (m_map[u] == Color::Null) {
            m_map[u] = Color::White;
        }

        if (m_map[v] == Color::Null) {
            m_map[v] = opposite(m_map[u]);
        } else if (m_map[v] == m_map[u]) {
            throw std::runtime_error("[bipartite] graph is not bipartite");
        }
    }

    using color_map = std::vector<Color>;
    color_map m_map;
};

int main() {
    ProblemArg problem;
    if (not (std::cin >> problem)) {
        return 1;
    }

    if (problem.n_vertecies <= 200) {
        detail::save_into_gv("./output/z4_graph.gv", detail::as_boost_graph(problem.graph));
    }

    try {
        algo::bfs(problem.graph, 0, BipartiteVisitor(problem.n_vertecies));
        std::println("[bipartite] graph is bipartite");
    } catch(std::runtime_error e) {
        std::println("{}", e.what());
    }
    return 0;
}
