#include <stack>
#include "asgt.h"

enum Color
{
  white,
  grey,
  black
};

// TODO update logic from stack o vertices to stack of edges, because that's what we're interessed in
// OR maybe use this logic to find cutvertices and from there find the bcc's 
void dfs(Graph &g, Vertex u, std::vector<int> &colours,
         std::vector<int> &d, std::vector<int> &f, std::vector<int> &low,
         std::vector<int> &pred, 
         std::stack<int> &v_stack, int *bcc_amount,
         int time)
{
  colours[u] = grey;
  d[u] = ++time;
  low[u] = d[u];
  v_stack.push(u);
  for (const auto &e : boost::make_iterator_range(boost::out_edges(u, g)))
  {
    int v = boost::target(e, g);
    if (colours[v] == white){
      pred[v] = u;
      dfs(g, v, colours, d, f, low, pred, v_stack, bcc_amount, time);
      // low[u] = min(low[u], low[v])
      low[u] = low[u] < low[v] ? low[u] : low[v]; 

    }else 
      // TODO check if it is a back arc 
      if (false) {
        // low[u] = min(low[u], d[v])
        low[u] = low[u] < d[v] ? low[u] : d[v];
    }
  }
  colours[u] = black;
  f[u] = ++time;
  // TODO check if its a base vertex
  if (false) {
    *bcc_amount = *bcc_amount + 1;
    // TODO pop vertices until 'u' is popped, and for each popped vertex, assing it's scc val to scc_amount
  }

}

void compute_bcc(Graph &g, bool fill_cutvxs, bool fill_bridges)
{
  int v_length = 0;
  for (const auto &vertex : boost::make_iterator_range(boost::vertices(g)))
  {
    v_length += 1;
  }
  std::vector<int> colours(v_length, white);
  std::vector<int> d(v_length, -1);
  std::vector<int> f(v_length, -1);
  std::vector<int> low(v_length, -1);
  std::vector<int> pred(v_length, -1);
  std::stack<int> v_stack;

  int bcc_amount = 0;
  int time = 0;
  for (const auto &u : boost::make_iterator_range(boost::vertices(g)))
  {
    if (colours[u] == white)
    {
      dfs(g, u, colours, d, f, low, pred, v_stack, &bcc_amount, time);
    }
  }
}
