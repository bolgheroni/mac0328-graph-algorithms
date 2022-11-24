/* this file MUST NOT be edited by the students */

#include "potential.h"

#include <cstdlib>              // for size_t

#include <algorithm>            // for std::all_of
#include <iomanip>              // for set::setw
#include <iostream>
#include <limits>
#include <string>               // for std::to_string
#include <vector>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/range/adaptors.hpp> // for boost::adaptors::indexed
#include <boost/range/iterator_range.hpp> // for boost::make_iterator_range

#include "digraph.h"
#include "potential-errors.h"

using boost::edges;
using boost::num_vertices;
using boost::source;
using boost::target;

using std::all_of;
using std::fixed;
using std::vector;

const double EPS = 1e-8;

static size_t count_digits(size_t n)
{
  return std::to_string(n).size();
}

/* FeasiblePotential constructor */
FeasiblePotential::FeasiblePotential(const Digraph& digraph,
                                     const vector<double> y)
  : _digraph(digraph),
    _y(y)
{
  if (y.size() != num_vertices(digraph)) {
    throw NotPotential_error(y.size(), num_vertices(digraph));
  }

  const auto& arcs = boost::make_iterator_range(edges(digraph));
  const auto& viol = find_if_not(arcs.begin(), arcs.end(),
                                 [&](const Arc& arc) -> bool {
                                   const Vertex& u = source(arc, digraph);
                                   const Vertex& v = target(arc, digraph);
                                   return _y[v] <= _y[u] + digraph[arc].cost + EPS;
                                 });
  if (viol != arcs.end()) {
    throw InfeasiblePotential_error(digraph, *viol, _y);
  }
}

std::ostream& operator<<(std::ostream& os,
                         const FeasiblePotential& feaspot)
{
  const vector<double>& y = feaspot._y;
  const vector<double>::size_type size = y.size();
  const size_t width = count_digits(size);
  os.precision(std::numeric_limits<double>::max_digits10);

  for (const auto& elt : y | boost::adaptors::indexed(1)) {
    os << "y[" << std::setw(width) << elt.index() << "] = "
       << std::showpos << fixed << elt.value() << std::noshowpos << '\n';
  }

  return os;
}

/* FeasibleMultiplier constructor */
FeasibleMultiplier::FeasibleMultiplier(const Digraph& digraph,
                                       const vector<double> z)
  : _digraph(digraph),
    _z(z)
{
  if (z.size() != num_vertices(digraph)) {
    throw  NotMultiplier_error(z.size(), num_vertices(digraph));
  }

  vector<double>::const_iterator
    nonpos = find_if_not (z.cbegin(), z.cend(),
                          [&](const double mult) -> bool {
                            return mult > 0.0;
                          });
  if (nonpos != z.cend()) {
    throw  InvalidMultiplier_error(nonpos - z.cbegin(), *nonpos);
  }

  const auto& arcs = boost::make_iterator_range(edges(digraph));
  const auto& viol = find_if_not(arcs.begin(), arcs.end(),
                                 [&](const Arc& arc) -> bool {
                                   const Vertex& u = source(arc, digraph);
                                   const Vertex& v = target(arc, digraph);
                                   return _z[v] >= (1 - EPS) * _z[u] * digraph[arc].cost;
                                 });
  if (viol != arcs.end()) {
    throw InfeasibleMultiplier_error(digraph, *viol, _z);
  }
}

std::ostream& operator<<(std::ostream& os,
                         const FeasibleMultiplier& feasmult)
{
  const vector<double>& z = feasmult._z;
  const vector<double>::size_type size = z.size();
  const size_t width = count_digits(size);
  os.precision(std::numeric_limits<double>::max_digits10);
  for (const auto& elt : z | boost::adaptors::indexed(1)) {
    os << "z[" << std::setw(width) << elt.index() << "] = "
       << std::showpos << fixed << elt.value() << std::noshowpos << '\n';
  }

  return os;
}
