/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector> // for std::vector

#ifndef DIGRAPH_H
#define DIGRAPH_H

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS>
    Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;

class HeadStart
{
public:
  HeadStart(int v_size) : d(v_size), f(v_size), by_f_time(v_size, -1) {}
  std::vector<int> d;
  std::vector<int> f;
  std::vector<int> by_f_time;
};

enum Color
{
  white,
  grey,
  black
};

#endif // #ifndef DIGRAPH_H

Digraph read_digraph(std::istream &in, int *ret_num_variables, int *ret_num_clauses)
{
  int num_variables, num_clauses;
  typename boost::graph_traits<Digraph>::vertices_size_type num_vertices;
  in >> num_variables;
  in >> num_clauses;

  num_vertices = num_variables * 2;

  *ret_num_variables = num_variables;
  *ret_num_clauses = num_clauses;

  std::cout << "Edges: "
            << "\n";
  // accumulate all data before graph construction
  std::vector<std::pair<int, int>> edges;
  while (num_clauses--)
  {
    int a, b;
    int u_1, v_1, u_2, v_2;
    in >> a >> b;
    std::cout << "From clause: (" << a << ", " << b << ")\n";
    a = -a;
    u_1 = a > 0 ? (a - 1) : (-a - 1 + num_variables);
    v_1 = b > 0 ? (b - 1) : (-b - 1 + num_variables);
    std::cout << "Made edges: ";
    std::cout << u_1 << " " << v_1 << " and ";
    edges.push_back(std::make_pair(u_1, v_1));

    a = -a;
    b = -b;
    u_2 = a > 0 ? a - 1 : (-a - 1) + num_variables;
    v_2 = b > 0 ? b - 1 : (-b - 1) + num_variables;
    std::cout << v_2 << " " << u_2 << "\n";
    edges.push_back(std::make_pair(v_2, u_2));
  }

  // guarantees copy elision in c++17
  return Digraph(edges.cbegin(), edges.cend(), num_vertices);
}

Digraph reverse_digraph(Digraph &dig, typename boost::graph_traits<Digraph>::vertices_size_type num_vertices)
{
  // accumulate all data before graph construction
  std::vector<std::pair<int, int>> edges;

  std::for_each(boost::edges(dig).first,
                boost::edges(dig).second,
                [&](const auto &arc)
                {
                  auto sourceV = boost::source(arc, dig);
                  auto targetV = boost::target(arc, dig);
                  edges.push_back(std::make_pair(targetV, sourceV));
                });
  return Digraph(edges.cbegin(), edges.cend(), num_vertices);
}

int dfs(Digraph &dig, const Vertex &current, std::vector<int> &d, std::vector<int> &f,
        std::vector<int> &by_f_time, std::vector<int> &colours,
        int time, int *current_f_time)
{
  colours[current] = grey;
  by_f_time[*current_f_time] = current;
  *current_f_time = *current_f_time + 1;
  d[current] = ++time;
  std::for_each(boost::out_edges(current, dig).first,
                boost::out_edges(current, dig).second,
                [&](const auto &arc)
                {
                  auto targetV = boost::target(arc, dig);
                  if (colours[targetV] == white)
                  {
                    time = dfs(dig, targetV, d, f, by_f_time, colours, time, current_f_time);
                  }
                });
  colours[current] = black;
  f[current] = ++time;
  return time;
}

void dfs_sc(Digraph &dig, int num_variables, const Vertex &current,
            std::vector<int> &colours, std::vector<int> &components,
            int currentComponent)
{
  colours[current] = grey;
  components[current] = currentComponent;

  std::for_each(boost::out_edges(current, dig).first,
                boost::out_edges(current, dig).second,
                [&](const auto &arc)
                {
                  auto targetV = boost::target(arc, dig);
                  if (colours[targetV] == white)
                  {
                    dfs_sc(dig, num_variables, targetV, colours, components, currentComponent);
                  }
                });
  colours[current] = black;
}

int dfs_reverse(Digraph &dig, int num_variables, const Vertex &current,
                std::vector<int> &d, std::vector<int> &f,
                std::vector<int> &d_old, std::vector<int> &f_old,
                std::vector<int> &colours, int time)
{
  colours[current] = grey;
  d[current] = ++time;
  int multiplier = 1;
  if (current >= num_variables)
    multiplier = -1;
  std::for_each(boost::out_edges(current, dig).first,
                boost::out_edges(current, dig).second,
                [&](const auto &arc)
                {
                  auto targetV = boost::target(arc, dig);
                  if (colours[targetV] == white)
                  {
                    // both literals for the same variable are in the same strong component
                    if (targetV == current + num_variables * (multiplier))
                    {
                      std::cout << targetV << " and " << current << " are in same SC..."
                                << "\n";
                      time = -1;
                    }
                    else
                    {
                      time = dfs_reverse(dig, num_variables, targetV, d, f, d_old, f_old, colours, time);
                    }
                  }
                });
  colours[current] = black;
  if (time == -1)
  {
    return -1;
  }
  f[current] = ++time;
  return time;
}

HeadStart preprocess(Digraph &dig, const Vertex &root)
{
  int v_length = 0;
  std::for_each(boost::vertices(dig).first,
                boost::vertices(dig).second,
                [&](const auto &vertex)
                {
                  v_length++;
                });
  auto ret = HeadStart(v_length);
  int time = 0;
  int current_f_time = 0;
  std::vector<int> colours(v_length, white);
  std::for_each(boost::vertices(dig).first,
                boost::vertices(dig).second,
                [&](const auto &vertex)
                {
                  if (colours[vertex] == white)
                    time = dfs(dig, vertex, ret.d, ret.f, ret.by_f_time, colours, time, &current_f_time);
                });
  return ret;
}

int check_validity(Digraph &rev, int num_variables,
                   std::vector<int> &d_old, std::vector<int> &f_old,
                   std::vector<int> &by_f_time)
{
  std::vector<int> colours(num_variables * 2, white);
  std::vector<int> d(num_variables * 2, white);
  std::vector<int> f(num_variables * 2, white);
  int time = 0;
  for (int i = 0; i < num_variables * 2; i++)
  {
    if (colours[by_f_time[i]] == white)
    {
      time = dfs_reverse(rev, num_variables, by_f_time[i],
                         d, f,
                         d_old, f_old,
                         colours, time) -
             1;
      if (time < 0)
      {
        return -1;
      }
    }
  }
  return 1;
}

std::vector<int> find_components(Digraph &rev, int num_variables,
                                 std::vector<int> &by_f_time, int lastTimeMarked)
{
  std::vector<int> colours(num_variables * 2, white);
  std::vector<int> components(num_variables * 2, white);
  int currentComponent = 0;
  for (int i = lastTimeMarked; i >= 0; i--)
  {
    if (colours[by_f_time[i]] == white)
    {
      dfs_sc(rev, num_variables, by_f_time[i],
             colours, components, currentComponent);
      currentComponent += 1;
    }
  }
  return components;
}

int main(int argc, char **argv)
{
  int debugLevel;
  std::cin >> debugLevel;
  std::cout << debugLevel << "\n";
  if (debugLevel != 0)
  {
    std::cout << "finish\n";
  }
  else
  {
    int num_variables, num_clauses;
    Digraph dig = read_digraph(std::cin, &num_variables, &num_clauses);
    HeadStart data = preprocess(dig, 0);

    Digraph rev = reverse_digraph(dig, num_variables * 2);
    std::cout << "\nResult from validity check: " << check_validity(rev, num_variables, data.d, data.f, data.by_f_time) << "\n";
    // size_t queries; std::cin >> queries;

    // while(queries--) {
    //   Vertex x, y; std::cin >> x >> y;
    //   std::cout << (is_ancestor (--x, --y, data) ? "YES" : "NO")
    //             << std::endl;
    // }
  }
  return EXIT_SUCCESS;
}
