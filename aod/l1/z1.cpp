#include <boost/program_options.hpp>
#include <iostream>
#include <print>
#include <string>

#include "common.hpp"

struct ProgramArgs {
    bool print_search_tree { false };
};

ProgramArgs parse_program_opts(int argc, char** argv)
{
    namespace po = boost::program_options;

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

    auto stree_bfs = algo::bfs(problem.graph, 0, [](std::size_t v) {
        std::println("[bfs] {}", v + 1);
    });

    std::println("");
    auto stree_dfs = algo::dfs(problem.graph, 0, [](std::size_t v) {
        std::println("[dfs] {}", v + 1);
    });

    detail::save_into_gv("output/z1_graph.gv", detail::as_boost_graph(problem.graph));
    if (args.print_search_tree) {
        detail::save_into_gv("output/z1_bfs.gv", stree_bfs);
        detail::save_into_gv("output/z1_dfs.gv", stree_dfs);
    }
    return 0;
}
