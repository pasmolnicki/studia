#include "common.hpp"
#include <iterator>
#include <vector>

struct SCCVertex {
    std::size_t idx;
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
        *m_output++ = SCCVertex { v, v };
    }

    OutputIt m_output;
};

using scc_vertex_conatiner = std::vector<SCCVertex>;
using scc_insert_it = std::back_insert_iterator<scc_vertex_conatiner>;

void scc_assign(std::size_t u, std::size_t root, const GraphAdjList& graph, scc_vertex_conatiner& conatiner)
{
    typename GraphAdjList::edge_container::value_type::const_iterator first;
    typename GraphAdjList::edge_container::value_type::const_iterator last;

    auto& vert = conatiner[u];
    vert.idx = u;
    if (not vert.assigned) {
        vert.assigned = true;
        vert.root = root;
        // std::println("root={} v={}", root + 1, u + 1);

        std::tie(first, last) = graph.edges(u);
        for (; first != last; first++) {
            // std::println("[children] v={} child={}", u+1, *first + 1);
            scc_assign(*first, root, graph, conatiner);
        }
    }
}

scc_vertex_conatiner strongly_connected_components(GraphAdjList& graph)
{
    scc_vertex_conatiner list;
    list.reserve(graph.n_vertecies());

    algo::dfs(graph, 0, SCCVisitor<scc_insert_it>(std::back_inserter(list)));
    std::reverse(list.begin(), list.end()); // TODO: fix this - produces index bug

    auto transp = graph.transpose();
    for (auto v : list) {
        scc_assign(v.idx, v.root, transp, list);
    }
    return list;
}

void print_results(const scc_vertex_conatiner& result)
{
    /*
    std::map<std::size_t, scc_vertex_conatiner> map;
    for (const auto& v : result) {
        if (map.find(v.root) == map.end()) {
            map[v.root] = scc_vertex_conatiner { };
        }
        map[v.root].push_back(v);
    }

    for (auto& [key, list] : map) {
        std::println("[scc][component] {}", key + 1);
        for (auto v : list) {
            std::println("\t\t\t {}", v.idx + 1);
        }
    }*/
    std::for_each(result.begin(), result.end(), [](SCCVertex v){
            std::println("[scc] {} -> {}", v.idx + 1, v.root + 1);
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
