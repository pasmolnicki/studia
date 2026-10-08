#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/program_options.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <print>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

using Graph = std::vector<std::vector<std::size_t>>;

struct ProgramArgs {
    bool print_search_tree { false };
};

struct ProblemArg {
    bool is_dag { false };
    std::size_t n_vertecies { };
    std::size_t n_edges { };
    Graph graph { };
};

namespace detail {
bool is_ok_read_dag_type(std::istream& in, ProblemArg& arg) noexcept(false)
{
    char type_tag;
    in >> type_tag;
    if (type_tag == 'D') {
        arg.is_dag = true;
    } else if (type_tag == 'U') {
        arg.is_dag = false;
    } else {
        std::println(std::cerr, "Invalid Graph type, expected 'D' or 'U'");
        in.setstate(std::istream::badbit);
        return false;
    }
    return true;
}

struct Vertex {
    int id;
    std::string label;
};

struct Edge {
};

using DirectedGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex,
    Edge>;
using VertexSpec = boost::graph_traits<DirectedGraph>::vertex_descriptor;
using EdgeSpec = boost::graph_traits<DirectedGraph>::edge_descriptor;

DirectedGraph
as_boost_graph(const Graph& graph)
{
    DirectedGraph g;
    std::vector<VertexSpec> vertecies;
    for (std::size_t i { 0 }; i < graph.size(); i++) {
        boost::add_vertex(Vertex { static_cast<int>(i), std::to_string(i) }, g);
    }

    std::size_t u { 0 };
    for (auto& list : graph) {
        for (auto& v : list) {
            boost::add_edge(u, v)
        }
        u++;
    }
}

void save_into_gv(std::string_view filename, const DirectedGraph& graph)
{
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&Vertex::id, graph));
    dp.property("label", boost::get(&Vertex::label, graph));

    std::ofstream out(filename.data());
    boost::write_graphviz_dp(out, graph, dp);
}

} // namespace detail

std::istream& operator>>(std::istream& in, ProblemArg& arg) noexcept(false)
{
    if (not detail::is_ok_read_dag_type(in, arg)) {
        return in;
    }

    if (not(in >> arg.n_vertecies)) {
        std::println(std::cerr, "Expected integer number of vertecies");
        return in;
    }
    if (not(in >> arg.n_edges)) {
        std::println(std::cerr, "Expected integer number of edges");
        return in;
    }

    auto& graph = arg.graph;
    graph.resize(arg.n_vertecies);

    for (std::size_t i { 0 }; i < arg.n_edges; i++) {
        std::size_t u, v;
        if (not(in >> u >> v)) {
            std::println(std::cerr, "Couldn't parse pair (u,v) with index={}", i);
            return in;
        }

        u--;
        v--;
        graph[u].push_back(v);

        // undirected - so it's both ways
        if (not arg.is_dag) {
            graph[v].push_back(u);
        }
    }

    return in;
}

namespace algo {
template <typename Visitor>
void bfs(const Graph& graph, std::size_t start, Visitor&& visitor)
{
    std::vector<bool> visited(graph.size(), false);
    std::queue<std::size_t> q;
    visited[start] = true;
    q.push(start);

    while (not q.empty()) {
        auto u = q.front();
        q.pop();

        visitor(u + 1);
        const auto& list = graph[u];
        std::for_each(list.begin(), list.end(), [&u, &visited, &q](auto v) {
            if (not visited[v]) {
                q.push(v);
                visited[v] = true;
            }
        });
    }
}

template <typename Visitor>
void dfs_visit(const Graph& graph, std::vector<bool>& visited, std::size_t vertex, Visitor&& visitor)
{
    visited[vertex] = true;
    visitor(vertex + 1);
    for (auto v : graph[vertex]) {
        if (not visited[v]) {
            dfs_visit(graph, visited, v, visitor);
        }
    }
}

template <typename Visitor>
void dfs(const Graph& graph, std::size_t start, Visitor&& visitor)
{
    std::vector<bool> visited(graph.size(), false);
    for (std::size_t i { 0 }; i < graph.size(); i++) {
        if (not visited[i]) {
            dfs_visit(graph, visited, i, visitor);
        }
    }
}

} // algo namespace

ProgramArgs parse_program_opts(int argc, char** argv)
{
    namespace po = boost::program_options;

    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()("help", "produce help message")("print-search-tree,p", "set pretty printing of the graph");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc;
        std::exit(0);
    }

    ProgramArgs args;
    if (vm.count("print-search-tree")) {
        args.print_search_tree = true;
    }
    return args;
}

int main(int argc, char** argv)
{
    auto args = parse_program_opts(argc, argv);
    ProblemArg problem;
    if (not(std::cin >> problem)) {
        return 1;
    }

    if (not args.print_search_tree) {
        algo::bfs(problem.graph, 0, [](std::size_t v) { std::println("[bfs] {}", v); });
        algo::dfs(problem.graph, 0, [](std::size_t v) { std::println("[dfs] {}", v); });
    } else {
        std::println(std::cerr, "Not implemented yet");
    }
    return 0;
}
