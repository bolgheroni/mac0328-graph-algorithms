#include "asgt.h"

void compute_bcc (Graph &g, bool fill_cutvxs, bool fill_bridges)
{
  /* fill everything with dummy values */
  for (const auto& vertex : boost::make_iterator_range(boost::vertices(g))) {
    g[vertex].cutvertex = false;
  }

  for (const auto& edge : boost::make_iterator_range(boost::edges(g))) {
    g[edge].bcc = 0;
    g[edge].bridge = false;
  }
}
