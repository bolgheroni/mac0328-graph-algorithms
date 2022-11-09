#include <stack>
#include "asgt.h"

enum Color
{
  white,
  grey,
  black
};

typedef boost::detail::edge_desc_impl<boost::undirected_tag, std::size_t> edge;

void pop_and_mark_edges(const edge &start_edge, std::stack<edge> &e_stack, Graph &g, int *bcc_amount)
{
  *bcc_amount = *bcc_amount + 1;
  edge w = e_stack.top();
  e_stack.pop();
  while (w != start_edge)
  {
    g[w].bcc = *bcc_amount;
    w = e_stack.top();
    e_stack.pop();
  }
  g[w].bcc = *bcc_amount;
}

void dfs(Graph &g, Vertex u,
         std::vector<size_t> &pred,
         std::stack<size_t> &v_stack, std::stack<edge> &e_stack,
         int *bcc_amount,
         bool u_is_root,
         int time)
{
  g[u].colour = grey;
  g[u].d = ++time;
  g[u].low = g[u].d;
  v_stack.push(u);
  g[u].in_stack = true;
  int descendants_amount = 0;

  for (const auto &e : boost::make_iterator_range(boost::out_edges(u, g)))
  {
    size_t v = boost::target(e, g);
    if (g[v].colour == white)
    {
      e_stack.push(e);
      descendants_amount += 1;
      pred[v] = u;
      dfs(g, v, pred, v_stack, e_stack, bcc_amount, false, time);
      // low[u] = min(low[u], low[v])
      g[u].low = g[u].low < g[v].low ? g[u].low : g[v].low;
      if (g[v].low >= g[u].d)
      {

        pop_and_mark_edges(e, e_stack, g, bcc_amount);

        g[u].cutvertex = true;
        if (g[v].low != g[u].d)
        {
          g[e].bridge = true;
        }
      }
    }
    else
      // if it is a back edge and doesnt just point back to the predecessor of u
      if (v != pred[u] && g[v].colour == grey && g[v].in_stack == true)
      {
        e_stack.push(e);

        // low[u] = min(low[u], d[v])
        g[u].low = g[u].low < g[v].d ? g[u].low : g[v].d;
      }
  }
  g[u].colour = black;
  g[u].f = ++time;
  if (g[u].d == g[u].low)
  {

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
  if (u_is_root)
  {
    g[u].cutvertex = descendants_amount >= 2;
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
  std::vector<size_t> pred(v_length, -1);
  std::stack<size_t> v_stack;
  std::stack<edge> e_stack;

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
