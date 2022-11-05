/* this file MUST NOT be edited by the students */

#ifndef ASGT_H
#define ASGT_H

#include "graph.h"

/* Fill in the field bcc for each edge appropriately. If either of the
 * fill_* parameters is set, then fill in the appropriate fields in
 * the vertices or edges. At most one of the fill_* parameters will be
 * set in any function call. */
void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges);

#endif // #ifndef ASGT_H
