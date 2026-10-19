
#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/property_map/dynamic_property_map.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <print>
#include <queue>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

struct GraphAdjList {
    enum Color : std::uint8_t {
        White,
        Gray,
        Black
    };

    struct Vertex {
        std::size_t idx;
        Color color;
    };

    template <typename T>
    using container = std::vector<T>;
    using vertex_container = container<Vertex>;
    using const_vertex_container = const container<Vertex>;
    using edge_container = container<container<std::size_t>>;
    using const_edge_container = const edge_container;

    GraphAdjList() = default;

    GraphAdjList(std::size_t n_vertecies) {
        add_verticies(n_vertecies);
    }

    std::pair<typename container<std::size_t>::iterator, typename container<std::size_t>::iterator> edges(const Vertex& v) {
        auto& edge = m_graph[v.idx];
        return std::make_pair(edge.begin(), edge.end());
    }

    std::pair<typename container<std::size_t>::const_iterator, typename container<std::size_t>::const_iterator> edges(const Vertex& v) const {
        auto& edge = m_graph[v.idx];
        return std::make_pair(edge.begin(), edge.end());
    }

    std::pair<typename container<std::size_t>::const_iterator, typename container<std::size_t>::const_iterator> edges(std::size_t i) const {
        auto& edge = m_graph[i];
        return std::make_pair(edge.begin(), edge.end());
    }

    void add_verticies(std::size_t n) {
        std::size_t i = m_verticies.size();
        m_verticies.resize(n + m_verticies.size());
        std::for_each(m_verticies.begin(), m_verticies.end(), [&i](Vertex& v){ v.idx = i++; });
        m_graph.resize(m_verticies.size());
    }

    void add_edge(std::size_t i, std::size_t j) {
        m_graph[i].push_back(j);
    }

    std::size_t n_vertecies() const {
        return m_verticies.size();
    }

    vertex_container& get_verticies() {
        return m_verticies;
    }

    const_vertex_container& get_verticies() const {
        return m_verticies;
    }

    edge_container& get_edges() {
        return m_graph;
    }

    const_edge_container& get_edges() const {
        return m_graph;
    }

    Vertex& vertex(std::size_t i) {
        return m_verticies[i];
    }

    const Vertex& vertex(std::size_t i) const {
        return m_verticies[i];
    }

    GraphAdjList transpose() const {
        GraphAdjList transp(n_vertecies());

        typename edge_container::value_type::const_iterator first;
        typename edge_container::value_type::const_iterator end;

        for (const auto& v : m_verticies) {
            std::tie(first, end) = edges(v);
            std::for_each(first, end, [&transp, &v](std::size_t idx){
                transp.add_edge(idx, v.idx);
            });
        }

        return transp;
    }

    vertex_container m_verticies;
    edge_container m_graph;
};

// using Graph = std::vector<std::vector<std::size_t>>;

struct ProblemArg {
    bool is_dag { false };
    std::size_t n_vertecies { };
    std::size_t n_edges { };
    GraphAdjList graph { };
};

namespace detail {
inline bool is_ok_read_dag_type(std::istream& in, ProblemArg& arg) noexcept(false)
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
    int id { -1 };
    std::string label { "null" };

    Vertex() = default;
    Vertex(std::size_t i)
        : id(static_cast<int>(i + 1))
        , label(std::to_string(i + 1))
    {
    }
};

struct Edge {
};

using DirectedGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex,
    Edge>;
using VertexSpec = boost::graph_traits<DirectedGraph>::vertex_descriptor;
using EdgeSpec = boost::graph_traits<DirectedGraph>::edge_descriptor;

[[nodiscard]] inline DirectedGraph as_boost_graph(const GraphAdjList& graph)
{
    DirectedGraph g;
    for (std::size_t i { 0 }; i < graph.get_verticies().size(); i++) {
        boost::add_vertex(Vertex(i), g);
    }

    std::size_t u { 0 };
    for (auto& list : graph.get_edges()) {
        for (auto& v : list) {
            boost::add_edge(boost::vertex(u, g), boost::vertex(v, g), Edge { }, g);
        }
        u++;
    }

    return g;
}

inline void save_into_gv(std::string_view filename, DirectedGraph g)
{
    // DirectedGraph g = as_boost_graph(graph);
    boost::dynamic_properties dp;
    dp.property("node_id", boost::get(&Vertex::id, g));
    dp.property("label", boost::get(&Vertex::label, g));
    std::ofstream out(filename.data());
    boost::write_graphviz_dp(out, g, dp);
}

} // namespace detail

inline std::istream& operator>>(std::istream& in, ProblemArg& arg) noexcept(false)
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
    graph.add_verticies(arg.n_vertecies);

    for (std::size_t i { 0 }; i < arg.n_edges; i++) {
        std::size_t u, v;
        if (not(in >> u >> v)) {
            std::println(std::cerr, "Couldn't parse pair (u,v) with index={}", i);
            return in;
        }

        u--;
        v--;
        auto n_v = graph.n_vertecies();
        if (u >= n_v || v >= n_v) {
            std::println(std::cerr, "Malformed edge ({}, {}), number of verticies={}", u, v, n_v);
            in.setstate(std::iostream::badbit);
            return in;
        }
        graph.add_edge(u, v);

        // undirected - so it's both ways
        if (not arg.is_dag) {
            graph.add_edge(v, u);
        }
    }

    return in;
}

namespace algo {

struct StackData {
    std::size_t boost_prev;
    std::optional<std::size_t> prev;
    std::size_t u;
};

struct BfsVisitor {
    virtual void enter_vertex(std::size_t v) { };
    virtual void visit_edge(std::size_t v, std::size_t u) { };
};

struct BfsFunctorVisitor : public BfsVisitor {
    std::function<void(std::size_t v)> func;

    virtual void enter_vertex(std::size_t v) {
        func(v);
    };

    BfsFunctorVisitor(std::function<void(std::size_t)> f)
        : func(f)
    {
    }
};

template <typename Visitor = BfsFunctorVisitor>
requires std::is_base_of_v<BfsVisitor, Visitor>
detail::DirectedGraph bfs(const GraphAdjList& graph, std::size_t start, Visitor& visitor)
{
    std::vector<bool> visited(graph.n_vertecies(), false);
    std::queue<StackData> q;
    visited[start] = true;
    q.push({ 0, start, start });
    detail::DirectedGraph stree;

    while (not q.empty()) {
        auto data = q.front();
        q.pop();

        visitor.enter_vertex(data.u);
        auto boost_idx = boost::add_vertex(detail::Vertex(data.u), stree);
        if (data.prev != data.u) {
            boost::add_edge(data.boost_prev, boost_idx, detail::Edge { }, stree);
        }

        auto [begin, end] = graph.edges(data.u);
        std::for_each(begin, end, [&visitor, &data, &visited, &q, &stree, &boost_idx, &graph](std::size_t i) {
            auto& v = graph.vertex(i);
            visitor.visit_edge(data.u, v.idx);
            if (not visited[v.idx]) {
                q.push({ boost_idx, data.u, v.idx });
                visited[v.idx] = true;
            }
        });
    }
    return stree;
}

template <typename Visitor = BfsFunctorVisitor>
requires std::is_base_of_v<BfsVisitor, Visitor>
detail::DirectedGraph bfs(const GraphAdjList& g, std::size_t start, Visitor&& vis) {
    return bfs(g, start, vis);
}

inline detail::DirectedGraph bfs(const GraphAdjList& g, std::size_t start, std::function<void(std::size_t)> fn){
    BfsFunctorVisitor vis{fn};
    return bfs(g, start, vis);
}

struct DfsVisitor {
    virtual void enter_vertex(std::size_t v) { };
    virtual void tree_edge(std::size_t u, std::size_t v) { };
    virtual void back_edge(std::size_t u, std::size_t v) { };
    virtual void finish_vertex(std::size_t v) { };
};

struct DfsPrintVisitor : public DfsVisitor {
    void enter_vertex(std::size_t v) override { std::println("[dfs] {}", v); }
};

struct DsfFunctorVisitor : public DfsVisitor {
    std::function<void(std::size_t v)> func;

    virtual void enter_vertex(std::size_t v) {
        func(v);
    };

    DsfFunctorVisitor(std::function<void(std::size_t)> f)
        : func(f)
    {
    }
};

template <typename Visitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
void dfs_visit(GraphAdjList::Vertex& v, std::size_t boost_prev, GraphAdjList& g, Visitor& visitor, detail::DirectedGraph& stree)
{
    v.color = GraphAdjList::Gray;
    visitor.enter_vertex(v.idx);

    std::size_t boost_idx = boost::add_vertex(detail::Vertex(v.idx), stree);
    if (boost_idx != boost_prev) {
        boost::add_edge(boost_prev, boost_idx, detail::Edge { }, stree);
    }

    auto [begin, end] = g.edges(v);
    for (; begin != end; begin++) {
        auto& next = g.vertex(*begin);
        auto c = next.color;
        if (c == GraphAdjList::White) {
            visitor.tree_edge(v.idx, next.idx);
            dfs_visit(next, boost_idx, g, visitor, stree);
        } else if (c == GraphAdjList::Gray) {
            visitor.back_edge(v.idx, next.idx);
        } // else forward edge
    }

    v.color = GraphAdjList::Black;
    visitor.finish_vertex(v.idx);
}

template <typename Visitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
detail::DirectedGraph impl_dfs_it(GraphAdjList& g, std::size_t start, Visitor& vis) {
    typename GraphAdjList::edge_container::value_type::iterator first;
    typename GraphAdjList::edge_container::value_type::iterator last;

    detail::DirectedGraph stree;
    std::vector<StackData> stack;

    stack.push_back({0, { }, start});
    while(not stack.empty()) {
        auto data= stack.front();
        stack.pop_back();
        GraphAdjList::Vertex& v = g.vertex(data.u);

        std::size_t boost_idx = boost::add_vertex(detail::Vertex(v.idx), stree);
        if (boost_idx != data.boost_prev) {
            boost::add_edge(data.boost_prev, boost_idx, detail::Edge { }, stree);
        }

        /*auto [begin, end] = g.edges(v);
        first = end - 1;
        last = begin - 1;*/
        std::tie(first, last) = g.edges(v);
        for (; first != last; ++first) {
            auto& u = g.vertex(*first);
            if (u.color == GraphAdjList::White) {
                vis.tree_edge(v.idx, u.idx);
                stack.push_back({boost_idx, v.idx, u.idx});
            }
            else if (u.color == GraphAdjList::Gray) {
                vis.back_edge(v.idx, u.idx);
            }
        }
    }
}

template <typename Visitor = DfsPrintVisitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
detail::DirectedGraph dfs(GraphAdjList& graph, std::size_t start, Visitor&& visitor)
{
    detail::DirectedGraph stree;
    dfs_visit(graph.vertex(start), 0, graph, visitor, stree);
    return stree;
}


template <typename Visitor = DfsPrintVisitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
detail::DirectedGraph dfs(GraphAdjList& graph, std::size_t start, Visitor& visitor)
{
    detail::DirectedGraph stree;
    dfs_visit(graph.m_verticies[start], 0, graph, visitor, stree);
    return stree;
}

inline detail::DirectedGraph dfs(GraphAdjList& graph, std::size_t start, std::function<void(std::size_t)> fn)
{
    return dfs(graph, start, DsfFunctorVisitor { fn });
}

template <typename Visitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
detail::DirectedGraph dfs_iterative(GraphAdjList& g, std::size_t start, Visitor&& vis) {
    return impl_dfs_it(g, start, vis);
}


template <typename Visitor>
    requires std::is_base_of_v<DfsVisitor, Visitor>
detail::DirectedGraph dfs_iterative(GraphAdjList& g, std::size_t start, Visitor& vis) {
    return impl_dfs_it(g, start, vis);
}

} // algo namespace
