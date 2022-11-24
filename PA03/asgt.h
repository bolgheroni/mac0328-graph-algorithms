/* this file MUST NOT be edited by the students */

#ifndef ASGT_H
#define ASGT_H

#include <tuple>

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/optional.hpp>

#include "digraph.h"
#include "cycle.h"
#include "potential.h"

/* Return a digraph built from market; the former digraph will be fed
 * as input to has_negative_cycle. This should run in O(m+n) time. */
Digraph build_digraph(const Digraph& market);

/* Return a triple formed by a boolean answer to the query "does
 * digraph have a negative cycle?", an optional NegativeCycle if the
 * answer is affirmative, and an optional Feasible if the answer is
 * negative. */
std::tuple<bool,
           boost::optional<NegativeCycle>,
           boost::optional<FeasiblePotential>> has_negative_cycle(Digraph& digraph);

/* Return a Loophole (in the market digraph) built from the cycle
 * negcycle in the auxiliary digraph aux_digraph. This should run in
 * O(m+n) time. */
Loophole build_loophole(const NegativeCycle& negcycle,
                        const Digraph& aux_digraph,
                        const Digraph& market);

/* Return a FeasibleMultiplier (in the market digraph) built from the
 * feasible potential feaspot in the auxiliary digraph aux_digraph.
 * This should run in O(n) time. */
FeasibleMultiplier build_feasmult(const FeasiblePotential& feaspot,
                                  const Digraph& aux_digraph,
                                  const Digraph& market);

#endif // #ifndef ASGT_H
