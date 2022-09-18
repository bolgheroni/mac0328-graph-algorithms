/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"
#include <vector>                      // for std::vector
#include <utility>                     // for std::pair

Arb read_arb(std::istream& in)
{
  typename boost::graph_traits<Arb>::vertices_size_type num_vertices; in >> num_vertices;

  Arb arb(num_vertices);
  size_t num_edges = num_vertices - 1;

  // accumulate all data before graph construction
  std::vector<std::pair<int, int> > edges;
  while (num_edges--) {
    int u, v; in >> u >> v;
    edges.push_back(std::make_pair(--u, --v));
  }

  // guarantees copy elision in c++17
  return Arb (edges.cbegin(), edges.cend(), num_vertices);
}

HeadStart preprocess (Arb &arb, const Vertex& root)
{
  return HeadStart(42);
}

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data)
{
  return false;
}
