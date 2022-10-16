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
  HeadStart(int v_size) : d(v_size), f(v_size), byTime(v_size, -1) {}
  std::vector<int> d;
  std::vector<int> f;
  std::vector<int> byTime;
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
    std::cout << u_2 << " " << v_2 << "\n";
    edges.push_back(std::make_pair(u_2, v_2));
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
        std::vector<int> &byTime, std::vector<int> &colours,
        int time)
{
  colours[current] = grey;
  byTime[time] = current;
  d[current] = ++time;
  std::for_each(boost::out_edges(current, dig).first,
                boost::out_edges(current, dig).second,
                [&](const auto &arc)
                {
                  auto targetV = boost::target(arc, dig);
                  if (colours[targetV] == white)
                  {
                    time = dfs(dig, targetV, d, f, byTime, colours, time);
                  }
                });
  colours[current] = black;
  f[current] = ++time;
  return time;
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
                      std::cout << targetV << " and " << current << " are in same SC..." <<"\n";
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
  std::cout << "n of vertices = " << v_length << "\n";
  auto ret = HeadStart(v_length);
  std::vector<int> colours(v_length, white);
  dfs(dig, root, ret.d, ret.f, ret.byTime, colours, 0);
  return ret;
}

int check_validity(Digraph &rev, int num_variables,
                   std::vector<int> &d_old, std::vector<int> &f_old,
                   std::vector<int> &byTime)
{
  std::vector<int> colours(num_variables * 2, white);
  std::vector<int> d(num_variables * 2, white);
  std::vector<int> f(num_variables * 2, white);
  int time = 0;
  for (int i = 0; i < num_variables * 2; i++)
  {
    if (colours[byTime[i]] == white)
    {
      time = dfs_reverse(rev, num_variables, byTime[i],
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
    std::cout << "\nResult from validity check: " << check_validity(rev, num_variables, data.d, data.f, data.byTime) << "\n";
    // size_t queries; std::cin >> queries;

    // while(queries--) {
    //   Vertex x, y; std::cin >> x >> y;
    //   std::cout << (is_ancestor (--x, --y, data) ? "YES" : "NO")
    //             << std::endl;
    // }
  }
  return EXIT_SUCCESS;
}
