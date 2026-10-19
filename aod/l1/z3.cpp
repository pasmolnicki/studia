#include "common.hpp"
#include <algorithm>
#include <iterator>
#include <vector>

struct SCCVertex {
    std::size_t vertex;
    std::size_t root;
    bool assigned { false };
};

template <typename OutputIt>
struct SCCVisitor : public algo::DfsVisitor {
    SCCVisitor(OutputIt it)
        : m_output(it)
    {
    }

    void finish_vertex(std::size_t v) override
    {
        // std::println("[scc] {}", v + 1);
        *m_output++ = v;
    }

    OutputIt m_output;
};

using scc_ordering_container = std::vector<std::size_t>;
using scc_vertex_conatiner = std::vector<SCCVertex>;
using scc_insert_it = std::back_insert_iterator<scc_ordering_container>;

void scc_assign(std::size_t u, std::size_t root, const GraphAdjList& graph, const scc_ordering_container& order, scc_vertex_conatiner& conatiner)
{
    typename GraphAdjList::edge_container::value_type::const_iterator first;
    typename GraphAdjList::edge_container::value_type::const_iterator last;

    auto& vert = conatiner[u];
    if (not vert.assigned) {
        vert.assigned = true;
        vert.root = root;
        vert.vertex = u;

        std::tie(first, last) = graph.edges(u);
        for (; first != last; first++) {
            scc_assign(*first, root, graph, order, conatiner);
        }
    }
}

scc_vertex_conatiner strongly_connected_components(GraphAdjList& graph)
{
    scc_ordering_container verticies;
    verticies.reserve(graph.n_vertecies());

    algo::dfs(graph, 0, SCCVisitor<scc_insert_it>(std::back_inserter(verticies)));
    std::reverse(verticies.begin(), verticies.end());

    scc_vertex_conatiner container(verticies.size(), SCCVertex { 0, 0, false });
    auto transp = graph.transpose();
    for (auto v : verticies) {
        scc_assign(v, v, transp, verticies, container);
    }
    return container;
}

void print_results(const scc_vertex_conatiner& result)
{
    std::for_each(result.begin(), result.end(), [](SCCVertex v) {
        std::println("[scc] {} -> {}", v.vertex + 1, v.root + 1);
    });
}

int main()
{
    ProblemArg problem;
    if (not(std::cin >> problem)) {
        return 1;
    }

    if (auto result = strongly_connected_components(problem.graph); problem.graph.n_vertecies() <= 200) {
        detail::save_into_gv("./output/z3_graph.gv", detail::as_boost_graph(problem.graph));
        print_results(result);
    }
    return 0;
}
