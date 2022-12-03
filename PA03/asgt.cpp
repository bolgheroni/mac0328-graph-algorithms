#include "asgt.h"

#include <utility> // for std::get
#include <tuple>
#include <vector>
#include <stack>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/optional.hpp>

#include "cycle.h"
#include "digraph.h"
#include "potential.h"

#include <math.h>

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

  std::for_each(boost::edges(market).first,
                boost::edges(market).second,
                [&](const auto &arc)
                {
                  auto sourceV = boost::source(arc, market);
                  auto targetV = boost::target(arc, market);
                  Arc a;
                  std::tie(a, std::ignore) = boost::add_edge(sourceV, targetV, digraph);
                  digraph[a].cost = (-1) * log10(market[arc].cost);
                });
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

NegativeCycle find_cicle(int vertex, std::vector<int> pi, Digraph &digraph)
{
  std::stack<int> path;
  int current = vertex;
  bool done = false;

  while (!done || current != vertex)
  {
    done = true;
    // std::cout << current + 1 << " ";
    path.push(current);
    current = pi[current];
  }
  path.push(current);
  // std::cout << current + 1 << "\n";

  Walk wk(digraph, vertex);

  current = path.top();
  path.pop();
  while (!path.empty())
  {
    int next = path.top();
    path.pop();
    // std::cout << current + 1 << " ";
    Arc a;
    std::tie(a, std::ignore) = boost::edge(current, next, digraph);

    wk.extend(a);
    current = next;
  }
  // std::cout << current + 1 << "\n";
  NegativeCycle cy(wk);
  return cy;
}

std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>>
has_negative_cycle(Digraph &digraph)
{
  int n_vertices = num_vertices(digraph);
  Digraph reverse = reverse_digraph(digraph, n_vertices);

  std::vector<double> d_l_v(n_vertices, std::numeric_limits<double>::infinity());
  // d_(l-1)_v
  std::vector<double> d_l_1_v(n_vertices, std::numeric_limits<double>::infinity());

  std::vector<int> pi(n_vertices);

  pi[0] = 0;

  d_l_1_v[0] = 0;
  d_l_v[0] = 0;

  for (int l = 1; l <= n_vertices; l++)
  {
    // std::cout << "Round " << l << "\n";
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(reverse)))
    {
      d_l_1_v[vertex] = d_l_v[vertex];
    }
    for (const auto &vertex : boost::make_iterator_range(boost::vertices(reverse)))
    {
      d_l_v[vertex] = d_l_1_v[vertex];
      // std::cout << "Vertex " << vertex + 1 << "\n";
      // had to reverse the digraph's edges source and target ends in order to use out_edges as "in_edges"
      for (const auto &edge : boost::make_iterator_range(boost::out_edges(vertex, reverse)))
      {
        // the boost::target is the source, sadly, due to the edges inversion
        auto source = boost::target(edge, reverse);
        double cost = reverse[edge].cost;
        // std::cout << source + 1 << ", " << vertex + 1 << " has cost " << cost << "+ " << d_l_1_v[source] << "\n";

        if (d_l_v[vertex] > d_l_1_v[source] + cost)
        {
          d_l_v[vertex] = d_l_1_v[source] + cost;
          pi[vertex] = source;
          // std::cout << vertex + 1 << ": " << d_l_v[vertex] << " new vs old " << d_l_1_v[vertex] << "\n";
        }
      }
    }
  }
  for (int vertex = 0; vertex < n_vertices; vertex++)
  {
    // std::cout << vertex + 1 << ": " << d_l_v[vertex] << " and " << d_l_1_v[vertex]
    //           << "\n";
    if (d_l_v[vertex] != d_l_1_v[vertex])
    {
      // std::cout << "Neg cycle at " << vertex + 1 << "\n";
      NegativeCycle cycle = find_cicle(vertex, pi, digraph);
      return {true, cycle, boost::none};
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
  Walk wk(market, boost::source(negcycle.get()[0], aux_digraph));

  for (Arc arc : negcycle.get())
  {
    Arc a;
    std::tie(a, std::ignore) = boost::edge(boost::source(arc, aux_digraph), boost::target(arc, aux_digraph), market);

    wk.extend(a);
  }
  return Loophole(wk);
}

FeasibleMultiplier build_feasmult(const FeasiblePotential &feaspot,
                                  const Digraph &aux_digraph,
                                  const Digraph &market)
{
  vector<double> z(num_vertices(market), 1.0);

  // encourage RVO
  return FeasibleMultiplier(market, z);
}
