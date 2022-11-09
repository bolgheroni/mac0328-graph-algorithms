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
void dfs(Graph &g, Vertex u,
         std::vector<int> &pred,
         std::stack<int> &v_stack, std::stack<std::pair<int, int>> &e_stack,
         int *bcc_amount,
         bool isRoot,
         int time)
{
  g[u].colour = grey;
  g[u].d = ++time;
  g[u].low = g[u].d;
  v_stack.push(u);
  g[u].in_stack = true;
  int descendantsAmount = 0;
  for (const auto &e : boost::make_iterator_range(boost::out_edges(u, g)))
  {
    int v = boost::target(e, g);
    if (g[v].colour == white)
    {
      descendantsAmount += 1;
      pred[v] = u;
      dfs(g, v, pred, v_stack, e_stack, bcc_amount, false, time);
      // low[u] = min(low[u], low[v])
      g[u].low = g[u].low < g[v].low ? g[u].low : g[v].low;
      if (g[v].low >= g[u].d)
      {
        g[u].cutvertex = true;
      }
    }
    else
      // if it is a back edge and doesnt point to the predecessor of u
      if (v != pred[u] && g[v].in_stack == true)
      {
        // low[u] = min(low[u], d[v])
        g[u].low = g[u].low < g[v].d ? g[u].low : g[v].d;
      }
  }
  g[u].colour = black;
  g[u].f = ++time;
  if (g[u].d == g[u].low)
  {
    *bcc_amount = *bcc_amount + 1;
    // TODO pop vertices until 'u' is popped, and for each popped vertex, assing it's scc val to scc_amount
    size_t v = v_stack.top();
    v_stack.pop();
    g[v].in_stack = false;
    while (v != u)
    {
      v = v_stack.top();
      v_stack.pop();
      g[v].in_stack = false;
    }
  }
  if (isRoot)
  {
    g[u].cutvertex = descendantsAmount >= 2;
  }
}

void compute_bcc(Graph &g, bool fill_cutvxs, bool fill_bridges)
{
  int v_length = 0;
  // initializing bundled vertices props and counting amount of vertices
  for (const auto &vertex : boost::make_iterator_range(boost::vertices(g)))
  {
    g[vertex].colour = white;
    g[vertex].d = -1;
    g[vertex].f = -1;
    g[vertex].low = -1;
    g[vertex].cutvertex = false;
    v_length += 1;
  }

  for (const auto &edge : boost::make_iterator_range(boost::edges(g)))
  {
    g[edge].bcc = 0;
    g[edge].bridge = false;
  }
  // predecessors array and vertices stack
  std::vector<int> pred(v_length, -1);
  std::stack<int> v_stack;
  std::stack<std::pair<int, int>> e_stack;

  // amount of biconnected components and algorithm 'time'
  int bcc_amount = 0;
  int time = 0;
  for (const auto &u : boost::make_iterator_range(boost::vertices(g)))
  {
    if (g[u].colour == white)
    {
      dfs(g, u, pred, v_stack, e_stack, &bcc_amount, true, time);
    }
  }
}
