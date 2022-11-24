/* this file MUST NOT be edited by the students */

#include "cycle.h"

#include <algorithm>            // for std::transform
#include <functional>           // for wrapper_reference and std::multiplies
#include <iostream>
#include <numeric>              // for std::accumulate
#include <utility>              // for pairs
#include <vector>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>

#include "digraph.h"
#include "cycle-errors.h"

using boost::num_vertices;
using boost::source;
using boost::target;

using std::accumulate;
using std::ostream;
using std::pair;
using std::vector;

const double EPS = 1e-8;

/* Walk constructor */
Walk::Walk(const Digraph& digraph, const Vertex& start)
  : _digraph(std::cref(digraph)),
    _start(start),
    _last(start),
    _path(true),
    _cycle(false),
    _visited(vector<bool>(num_vertices(digraph), false))
{
  _visited[start] = true;
}

bool Walk::extend(const Arc& arc)
{
  Vertex u = source(arc, digraph());
  if (u != _last) {
    return false;
  }
  Vertex v = target(arc, digraph());
  _arcs.push_back(arc);

  _cycle = false;

  if (!_visited[v]) {
    _visited[v] = true;
  } else {
    if (_path && v == _start) {
      _cycle = true;
    }
    _path = false;
  }

  _last = v;

  return true;
}

ostream& operator<<(ostream& os, const Walk& walk)
{
  const Digraph& digraph = walk.digraph();
  const vector<Arc>& arcs = walk._arcs;
  vector<Arc>::const_iterator a_it = arcs.cbegin();
  if (a_it == arcs.cend()) {
    return os;
  }
  Vertex v = source(*a_it, digraph);
  os << "Walk = (vx" << v+1 << ")";
  while (a_it != arcs.cend()) {
    const Arc& a = *a_it;
    os << std::showpos
       << " --[arc " << digraph[a].cost << "]-> "
       << std::noshowpos
       << "(vx" << target(a, digraph) + 1 << ")";
    ++a_it;
  }
  os << '\n';
  return os;
}

/* Cycle constructor */
Cycle::Cycle(const Walk& cycle)
  : _digraph(std::cref(cycle.digraph())),
    _arcs(cycle.arcs())
{
  if (!cycle.is_cycle()) {
    throw NotCycle_error(cycle);
  }
}

ostream& operator<<(ostream& os, const Cycle& cycle)
{
  const Digraph& digraph = cycle.digraph();
  const vector<Arc>& arcs = cycle.get();
  vector<Arc>::const_iterator a_it = arcs.cbegin();
  if (a_it == arcs.cend()) {
    return os;
  }
  Vertex v = source(*a_it, digraph);
  os << "Cycle = (vx" << v+1 << ")";
  while (a_it != arcs.cend()) {
    const Arc& a = *a_it;
    os << std::showpos
       << " --[arc " << digraph[a].cost << "]-> "
       << std::noshowpos
       << "(vx" << target(a, digraph) + 1 << ")";
    ++a_it;
  }
  os << '\n';
  return os;
}

/* NegativeCycle constructor */
NegativeCycle::NegativeCycle(const Walk& cycle)
  : Cycle(cycle)
{
  const Digraph& digraph = _digraph.get();
  vector<double> costs(_arcs.size());
  std::transform(_arcs.cbegin(), _arcs.cend(),
                 costs.begin(),
                 [&](const auto& arc) -> double {
                   return digraph[arc].cost;
                 });
  _cost = accumulate(costs.cbegin(), costs.cend(), 0.0);
  if (_cost >= EPS) {
    throw NonnegativeCycle_error(static_cast<Cycle>(*this), _cost);
  }
}

/* Loophole constructor */
Loophole::Loophole(const Walk& cycle)
  : Cycle(cycle)
{
  const Digraph& digraph = _digraph.get();
  vector<double> yields(_arcs.size());
  std::transform(_arcs.cbegin(), _arcs.cend(),
                 yields.begin(),
                 [&](const auto& arc) -> double {
                   return digraph[arc].cost;
                 });
  _yield = accumulate(yields.cbegin(), yields.cend(), 1.0,
                      std::multiplies<double>());
  if (_yield <= 1.0 - EPS) {
    throw NonLoophole_error(static_cast<Cycle>(*this), _yield);
  }
}
