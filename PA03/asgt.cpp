#include "asgt.h"

#include <utility> // for std::get
#include <tuple>
#include <vector>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

/* The code in this template file is all "bogus". It just illustrates
 * how to return answers back to main.cpp. */

/* The following declarations shorten the bogus code below. Feel free
 * to change/drop them. */
using boost::add_edge;
using boost::num_vertices;
using boost::out_edges;
using std::vector;

Digraph build_digraph(const Digraph &market)
{
  /* placeholder for NRVO */
  Digraph digraph(num_vertices(market));

  /* flip some signs in the arc costs below to exercise the many
   * execution pathways */

  /* create arcs 01 and 10 */
  Arc a0, a1, a2, a3, a4;

  std::tie(a4, std::ignore) = add_edge(0, 3, digraph);
  digraph[a4].cost = 3.0;

  std::tie(a3, std::ignore) = add_edge(3, 1, digraph);
  digraph[a3].cost = 9.0;

  // THE NEG CYCLE
  std::tie(a0, std::ignore) = add_edge(0, 1, digraph);
  digraph[a0].cost = 11.0;

  std::tie(a1, std::ignore) = add_edge(1, 2, digraph);
  digraph[a1].cost = -17.0;

  std::tie(a2, std::ignore) = add_edge(2, 0, digraph);
  digraph[a2].cost = -17.0;

  return digraph;
}

Digraph reverse_digraph(Digraph &dig, typename boost::graph_traits<Digraph>::vertices_size_type num_vertices)
{
  Digraph reverse(num_vertices);

  std::for_each(boost::edges(dig).first,
                boost::edges(dig).second,
                [&](const auto &arc)
                {
                  auto sourceV = boost::source(arc, dig);
                  auto targetV = boost::target(arc, dig);
                  Arc a;
                  std::tie(a, std::ignore) = boost::add_edge(targetV, sourceV, reverse);
                  reverse[a].cost = dig[arc].cost;
                });
  return reverse;
}

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph &digraph)
{
  int n_vertices = 0;

  for (const auto &vertex : boost::make_iterator_range(boost::vertices(digraph)))
  {
    n_vertices += 1;
  }
  Digraph reverse = reverse_digraph(digraph, n_vertices);

  std::vector<int> d_l_v(n_vertices, INFINITY);
  // d_(l-1)_v
  std::vector<int> d_l_1_v(n_vertices, INFINITY);
  d_l_1_v[0] = 0;
  d_l_v[0] = 0;

  for (int l = 1; l <= n_vertices; l++)
  {
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(reverse)))
    {
      d_l_1_v[vertex] = d_l_v[vertex];
    }
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(reverse)))
    {
      d_l_v[vertex] = d_l_1_v[vertex];
      std::cout << "Vertex " << vertex + 1 << "\n";
      // had to reverse the digraph's edges source and target ends in order to use out_edges as "in_edges"
      for (const auto &edge : boost::make_iterator_range(boost::out_edges(vertex, reverse)))
      {
        // the boost::target is the source, sadly, due to the edges inversion
        auto source = boost::target(edge, reverse);
        double cost = reverse[edge].cost;
        std::cout << source + 1 << ", " << vertex + 1 << " has cost " << cost << "\n";

        if (d_l_v[vertex] > d_l_1_v[source] + cost)
        {
          d_l_v[vertex] = d_l_1_v[source] + cost;
          if (l == n_vertices)
          {
            std::cout << "Neg cycle at " << vertex + 1 << "\n";
            return {true, boost::none, boost::none};
          }
        }
      }
    }
  }
  std::cout << "No Neg cycle "
               "\n";

  return {false, boost::none, boost::none};
}

Loophole build_loophole(const NegativeCycle &negcycle,
                        const Digraph &aux_digraph,
                        const Digraph &market)
{
  /* bogus code */
  const Arc &b0 = *(out_edges(0, market).first);
  const Arc &b1 = *(out_edges(1, market).first);

  Walk w(market, 0);
  w.extend(b0);
  w.extend(b1);

  // encourage RVO
  return Loophole(w);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential &feaspot,
                                  const Digraph &aux_digraph,
                                  const Digraph &market)
{
  vector<double> z(num_vertices(market), 1.0);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
