#include <iostream>
#include <iterator>
#include <print>
#include <utility>
#include <boost/graph/topological_sort.hpp>
#include <vector>

#include "common.hpp"

template <typename OutputIt>
struct DfsTopoVisitor : public algo::DfsVisitor {
    OutputIt m_output;
    bool m_has_cycle { false };

    DfsTopoVisitor(OutputIt it)
        : m_output(it)
    {
    }

    void back_edge(std::size_t u, std::size_t v) override
    {
        m_has_cycle = true;
    }

    void finish_vertex(std::size_t v) override
    {
        *m_output++ = v;
    }
};

using conatiner = std::vector<std::size_t>;
using insert_it = std::back_insert_iterator<conatiner>;

std::pair<conatiner, bool> topological_sort(GraphAdjList& graph)
{
    conatiner verticies;
    verticies.reserve(graph.n_vertecies());
    DfsTopoVisitor<insert_it> vis(std::back_inserter(verticies));
    algo::dfs(graph, 0, vis);
    std::reverse(verticies.begin(), verticies.end());
    return std::make_pair(verticies, vis.m_has_cycle);
}

int main(int argc, char** argv)
{
    std::println("[topo] loading problemset");
    ProblemArg problem;
    if (not(std::cin >> problem)) {
        return 1;
    }

    if (problem.graph.n_vertecies() <= 200) {
        std::println("[topo] saving graph");
        detail::save_into_gv("output/z2_graph.gv", detail::as_boost_graph(problem.graph));
    }

    conatiner order;
    bool has_cycle;
    std::println("[topo] running topological sort");
    std::tie(order, has_cycle) = topological_sort(problem.graph);

    auto g = detail::as_boost_graph(problem.graph);

    if (has_cycle) {
        std::println("[topo] graph has cycle");
    } else {
        std::println("[topo] graph doesn't contain any cycles");
        if (problem.graph.n_vertecies() <= 200) {
            std::for_each(order.begin(), order.end(), [](std::size_t v) {
                std::println("[topo][order] {}", v + 1);
            });
        }
    }

    /*std::println();
    std::vector<detail::DirectedGraph::vertex_descriptor> c;
    boost::topological_sort(g, std::back_inserter(c));
    for (auto begin = c.rbegin(); begin != c.rend(); begin++) {
        std::println("[topo][boost] {}", *begin + 1);
    }*/

    std::println("[topo] done");
    return 0;
}
