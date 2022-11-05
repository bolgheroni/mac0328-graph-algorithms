/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/range/iterator_range.hpp> // for boost::make_iterator_range
#include "asgt.h"

int main(int argc, char** argv)
{
  int debug; std::cin >> debug;

  typename boost::graph_traits<Graph>::vertices_size_type n; std::cin >> n;

  Graph graph(n);

  size_t m; std::cin >> m;

  while (m--) {
    int u, v; std::cin >> u >> v;
    boost::add_edge(--u, --v, graph);
  }

  switch (debug) {
  case 0:
    {
      compute_bcc(graph, false, false);
      for (const auto& edge : boost::make_iterator_range(boost::edges(graph))) {
        auto u = boost::source(edge, graph) + 1;
        auto v = boost::target(edge, graph) + 1;
        std::cout << "bcc[" << u << "," << v << "] = " << graph[edge].bcc << std::endl;
      }
      break;
    }
  case 1:
    compute_bcc(graph, true, false);
    std::cout << std::boolalpha;
    for (const auto& vertex : boost::make_iterator_range(boost::vertices(graph))) {
      std::cout << "cutvx[" << vertex + 1 << "] = " << graph[vertex].cutvertex << std::endl;
    }
    break;
  case 2:
    compute_bcc(graph, false, true);
    std::cout << std::boolalpha;
    for (const auto& edge : boost::make_iterator_range(boost::edges(graph))) {
      auto u = boost::source(edge, graph) + 1;
      auto v = boost::target(edge, graph) + 1;
      std::cout << "bridge[" << u << "," << v << "] = " << graph[edge].bridge << std::endl;
    }
    break;
  default:
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
