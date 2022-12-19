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

Digraph read_digraph(std::istream &is);

Digraph read_digraph(std::istream &is)
{
    typename boost::graph_traits<Digraph>::vertices_size_type n;
    is >> n;

    Digraph digraph(n);

    size_t m;
    is >> m;

    while (m--)
    {
        int u, v;
        is >> u >> v;
        Arc a;
        std::tie(a, std::ignore) = boost::add_edge(--u, --v, digraph);
        is >> digraph[a].cost;
    }

    return digraph;
}

int main(int argc, char **argv)
{
    try
    {
        Digraph digraph{read_digraph(std::cin)};
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}