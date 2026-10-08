#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <fstream>

struct Vertex {
    int some_int;
    std::string name;
};

struct Edge {
    // double weight;
};

using DirectedSimpleGraph = boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, Vertex,
    Edge>;
using VertexSpec = boost::graph_traits<DirectedSimpleGraph>::vertex_descriptor;
using EdgeSpec = boost::graph_traits<DirectedSimpleGraph>::edge_descriptor;

int main()
{
    DirectedSimpleGraph graph;
    boost::dynamic_properties dp;

    // "node_id", "label", and "weight" are reserved keywords
    // of the GraphViz DOT Format
    dp.property("node_id", boost::get(&Vertex::some_int, graph));
    dp.property("label", boost::get(&Vertex::name, graph));
    // dp.property("weight", boost::get(&Edge::weight, graph));

    // Create vertices with id and label
    VertexSpec v0 = boost::add_vertex(Vertex { 0, "Zero" }, graph);
    VertexSpec v1 = boost::add_vertex(Vertex { 1, "One" }, graph);
    VertexSpec v2 = boost::add_vertex(Vertex { 2, "Two" }, graph);
    VertexSpec v3 = boost::add_vertex(Vertex { 3, "Three" }, graph);

    // Create edges with weight
    boost::add_edge(v0, v1, Edge { }, graph);
    boost::add_edge(v0, v2, Edge { }, graph);
    boost::add_edge(v2, v3, Edge { }, graph);

    // Write to file
    std::ofstream graph_file_out("./out.gv");
    boost::write_graphviz_dp(graph_file_out, graph, dp);
}
