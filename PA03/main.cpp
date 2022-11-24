/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <cstdlib>              // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <exception>
#include <iostream>
#include <tuple>                // for std::tie and std::ignore
#include <utility>              // for pairs

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "asgt.h"
#include "cycle.h"
#include "digraph.h"
#include "potential.h"

using boost::optional;

using std::cout;
using std::endl;
using std::flush;

Digraph read_market(std::istream& is);

Digraph read_market(std::istream& is)
{
  typename boost::graph_traits<Digraph>::vertices_size_type n; is >> n;

  Digraph market(n);

  size_t m; is >> m;

  while (m--) {
    int u, v; is >> u >> v;
    Arc a;
    std::tie(a, std::ignore) = boost::add_edge(--u, --v, market);
    is >> market[a].cost;
  }

  return market;
}

int main(int argc, char** argv)
{
  Digraph market{read_market(std::cin)};
  Digraph digraph{build_digraph(market)};

  try {
    cout.precision(16);
    auto ret = has_negative_cycle(digraph);
    if (std::get<bool>(ret)) {
      const auto& opt_negcycle = std::get<optional<NegativeCycle>>(ret);
      if (!opt_negcycle) {
        cout << "Negative cycle reported but not computed" << endl;
      } else {
        const NegativeCycle& negcycle = opt_negcycle.value();
        if (&(negcycle.digraph()) != &digraph) {
          cout << "Cycle built on the wrong digraph" << endl;
        } else {
          cout << "Negative cycle found with cost "
               << negcycle.cost() << endl;
          cout << negcycle << std::flush;
          Loophole loophole{build_loophole(negcycle, digraph, market)};
          if (&(loophole.digraph()) != &market) {
            cout << "Cycle built on the wrong digraph" << endl;
          } else {
            cout << "Loophole found with yield "
                 << loophole.yield() << endl;
            cout << loophole << std::flush;
          }
        }
      }
    } else {
      const auto& opt_feaspot = std::get<optional<FeasiblePotential>>(ret);
      if (!opt_feaspot) {
        cout << "Feasible potential reported but not computed" << endl;
      } else {
        const FeasiblePotential& feaspot = opt_feaspot.value();
        if (&(feaspot.digraph()) != &digraph) {
          cout << "Feasible potential built on the wrong digraph" << endl;
        } else {
          cout << "Feasible potential found" << endl;
          cout << feaspot;
          FeasibleMultiplier feasmult{build_feasmult(feaspot, digraph, market)};
          if (&(feasmult.digraph()) != &market) {
            cout << "Feasible multipliers built on the wrong digraph" << endl;
          } else {
            cout << "Feasible multipliers found" << endl;
            cout << feasmult;
          }
        }
      }
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
