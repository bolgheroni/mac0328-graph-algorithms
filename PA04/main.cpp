#include <cstdlib> // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <exception>
#include <iostream>
#include <tuple>   // for std::tie and std::ignore
#include <utility> // for pairs

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "digraph.h"

using boost::optional;

using std::cout;
using std::endl;
using std::flush;

std::tuple<size_t, size_t, size_t, size_t> get_digraph_metadata(std::istream &is)
{
    size_t source;
    size_t target;
    size_t n;
    size_t m;

    is >> n;
    is >> m;

    is >> source;
    source -= 1;
    is >> target;
    target -= 1;

    std::cout << "Source: " << source + 1 << "\n";
    std::cout << "Target: " << target + 1 << "\n";

    return std::make_tuple(n, m, source, target);
}

Digraph read_digraph(std::istream &is, typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m);

Digraph read_digraph(std::istream &is, typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m)
{
    Digraph digraph(n);

    while (m--)
    {
        int u, v;
        is >> u >> v;
        Arc a;
        std::tie(a, std::ignore) = boost::add_edge(--u, --v, digraph);
        is >> digraph[a].cost;
        std::cout << u + 1 << "->" << v + 1 << "(" << digraph[a].cost << ")\n";
    }

    return digraph;
}

int main(int argc, char **argv)
{
    try
    {

        size_t n, m, source, target;
        std::tie(n, m, source, target) = get_digraph_metadata(std::cin);
        Digraph digraph{read_digraph(std::cin, n, m)};
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}