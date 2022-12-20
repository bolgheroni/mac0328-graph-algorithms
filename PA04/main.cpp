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

// Digraph read_digraph(std::istream &is, typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m);

std::tuple<size_t, int, size_t, size_t> get_digraph_metadata(std::istream &is)
{
    size_t source;
    size_t target;
    size_t n;
    int m;

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

std::vector<std::tuple<size_t, size_t, int>> get_digraph_arcs(std::istream &is, int m)
{
    std::vector<std::tuple<size_t, size_t, int>> arcs(0);

    while (m--)
    {
        int u, v;
        int capacity;
        is >> u >> v;
        is >> capacity;
        arcs.push_back(std::make_tuple(--u, --v, capacity));
        // std::cout << u + 1 << "->" << v + 1 << "(" << capacity << ")\n";
    }
    return arcs;
}

Digraph build_digraph(typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m, std::vector<std::tuple<size_t, size_t, int>> arcs)
{
    Digraph digraph(n);
    for (auto const &arc : arcs)
    {
        int u, v, capacity;
        std::tie(u, v, capacity) = arc;
        Arc a;
        std::tie(a, std::ignore) = boost::add_edge(u, v, digraph);
        digraph[a].capacity = capacity;
        std::cout << u + 1 << "->" << v + 1 << "(" << capacity << ")\n";
    }

    return digraph;
}

Digraph compute_residual(Digraph &digraph)
{
    Digraph residual(boost::num_vertices(digraph));

    std::for_each(boost::edges(digraph).first,
                  boost::edges(digraph).second,
                  [&](const auto &arc)
                  {
                      auto sourceV = boost::source(arc, digraph);
                      auto targetV = boost::target(arc, digraph);

                      if (digraph[arc].capacity - digraph[arc].flow > 0)
                      {
                          Arc a;
                          std::tie(a, std::ignore) = boost::add_edge(sourceV, targetV, residual);
                          residual[a].is_backwards = false;
                          residual[a].capacity = digraph[arc].capacity;
                          residual[a].flow = digraph[arc].flow;
                          residual[a].res_capacity = digraph[arc].capacity - digraph[arc].flow;
                      }

                      if (digraph[arc].flow > 0)
                      {
                          Arc b;
                          std::tie(b, std::ignore) = boost::add_edge(targetV, sourceV, residual);
                          residual[b].is_backwards = true;
                          residual[b].capacity = digraph[arc].capacity;
                          residual[b].flow = digraph[arc].flow;
                          residual[b].res_capacity = digraph[arc].flow;
                      }
                  });
    return residual;
}

void print_residual_capacities(Digraph &digraph, std::vector<std::tuple<size_t, size_t, int>> arcs)
{
    std::cout << "Residual capacities: \n";

    for (auto const &arc : arcs)
    {
        int u, v, capacity;
        std::tie(u, v, capacity) = arc;
        Arc a;
        std::tie(a, std::ignore) = boost::edge(u, v, digraph);
        // std::cout << u + 1 << "->" << v + 1 << " (" << 1 << ") = " << digraph[a].capacity - digraph[a].flow << " \n";
        // std::cout << u + 1 << "->" << v + 1 << " (" << -1 << ") = " << digraph[a].flow << " \n\n";
        std::cout << digraph[a].capacity - digraph[a].flow << " " << digraph[a].flow << " \n";
    }

}

int max_integral_flow(Digraph &digraph, std::vector<std::tuple<size_t, size_t, int>> arcs, size_t source, size_t target)
{
    for (int t = 0; t <= 5; t++)
    {
        print_residual_capacities(digraph, arcs);
        Digraph residual = compute_residual(digraph);
        //  ...
    }
    return 1;
}

int main(int argc, char **argv)
{
    try
    {

        size_t n, source, target;
        int m;
        std::tie(n, m, source, target) = get_digraph_metadata(std::cin);
        const std::vector<std::tuple<size_t, size_t, int>> arcs = get_digraph_arcs(std::cin, m);
        Digraph digraph{build_digraph(n, m, arcs)};
        max_integral_flow(digraph, arcs, source, target);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}