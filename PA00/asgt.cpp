/* This is the main file that the students must work on to submit; the
 * other one is arb.h
 */

#include "asgt.h"
#include <vector>  // for std::vector
#include <utility> // for std::pair

enum Color
{
  white,
  grey,
  black
};


Arb read_arb(std::istream &in)
{
  typename boost::graph_traits<Arb>::vertices_size_type num_vertices;
  in >> num_vertices;

  Arb arb(num_vertices);
  size_t num_edges = num_vertices - 1;

  // accumulate all data before graph construction
  std::vector<std::pair<int, int>> edges;
  while (num_edges--)
  {
    int u, v;
    in >> u >> v;
    edges.push_back(std::make_pair(--u, --v));
  }

  // guarantees copy elision in c++17
  return Arb(edges.cbegin(), edges.cend(), num_vertices);
}


int dfs(Arb &arb, const Vertex &current, std::vector<int> &d, std::vector<int> &f, std::vector<int> &colours, int time)
{
  colours[current] = grey;
  d[current] = ++time;
  std::for_each(boost::out_edges(current, arb).first,
                                boost::out_edges(current, arb).second,
                                [&](const auto& arc) {
                                  auto targetV = boost::target(arc, arb); 
                                  if (colours[targetV] == white) {
                                    time = dfs(arb, targetV, d, f, colours, time);
                                  }
                                });
  colours[current] = black;
  f[current] = ++time;
  return time; 
}

HeadStart preprocess(Arb &arb, const Vertex &root)
{
  int v_length = 0;
  std::for_each(boost::vertices(arb).first,
                boost::vertices(arb).second,
                [&](const auto &vertex)
                {
                  v_length++;
                });

  auto ret = HeadStart(v_length);
  std::vector<int> colours(v_length, white);
  dfs(arb, root, ret.d, ret.f, colours, 0);
  return ret;
}

bool is_ancestor(const Vertex &u, const Vertex &v, const HeadStart &data)
{
  return data.d[u] <= data.d[v] && data.f[v] <= data.f[u];
}
