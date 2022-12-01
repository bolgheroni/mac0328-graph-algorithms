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
  Arc a0, a1, a2;
  std::tie(a0, std::ignore) = add_edge(1, 0, digraph);
  digraph[a0].cost = 11.0;

  std::tie(a1, std::ignore) = add_edge(2, 1, digraph);
  digraph[a1].cost = -17.0;

  std::tie(a2, std::ignore) = add_edge(0, 2, digraph);
  digraph[a2].cost = -17.0;

  return digraph;
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

  std::vector<int> d_l_v(n_vertices, INFINITY);
  // d_(l-1)_v
  std::vector<int> d_l_1_v(n_vertices, INFINITY);
  d_l_1_v[0] = 0;
  d_l_v[0] = 0;

  for (int l = 1; l <= n_vertices; l++)
  {
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(digraph)))
    {
      d_l_1_v[vertex] = d_l_v[vertex];
    }
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(digraph)))
    {
      d_l_v[vertex] = d_l_1_v[vertex];
      std::cout << "Edge " << vertex + 1 << "\n";
      // had to reverse the edges source and target in order to use out_edges as "in_edges"
      for (const auto &edge : boost::make_iterator_range(boost::out_edges(vertex, digraph)))
      {
        // the boost::target is the source, sadly, due to the edges inversion
        auto source = boost::target(edge, digraph);

        std::cout << source + 1 << ", " << vertex + 1 << "\n";
        if (d_l_v[vertex] > d_l_1_v[source] + digraph[edge].cost)
        {
          d_l_v[vertex] = d_l_1_v[source] + digraph[edge].cost;
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
