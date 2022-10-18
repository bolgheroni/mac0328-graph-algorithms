/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <vector> // for std::vector
#include <string>

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
  int latest_f_time;
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

  // std::cout << "Edges: "
  // << "\n";
  // accumulate all data before graph construction
  std::vector<std::pair<int, int>> edges;
  while (num_clauses--)
  {
    int a, b;
    int u_1, v_1, u_2, v_2;
    in >> a >> b;
    // std::cout << "From clause: (" << a << ", " << b << ")\n";
    a = -a;
    u_1 = a > 0 ? (a - 1) : (-a - 1 + num_variables);
    v_1 = b > 0 ? (b - 1) : (-b - 1 + num_variables);
    // std::cout << "Made edges: ";
    // std::cout << u_1 << " " << v_1 << " and ";
    edges.push_back(std::make_pair(u_1, v_1));

    a = -a;
    b = -b;
    u_2 = a > 0 ? a - 1 : (-a - 1) + num_variables;
    v_2 = b > 0 ? b - 1 : (-b - 1) + num_variables;
    // std::cout << v_2 << " " << u_2 << "\n";
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
  by_f_time[*current_f_time] = current;
  *current_f_time = *current_f_time + 1;
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
  ret.latest_f_time = current_f_time - 1;
  return ret;
}

std::vector<int> find_components(Digraph &rev, int num_variables,
                                 std::vector<int> &by_f_time, int latest_f_time, int *num_components)
{
  std::vector<int> colours(num_variables * 2, white);
  std::vector<int> components(num_variables * 2, white);
  int currentComponent = 0;
  for (int i = latest_f_time; i >= 0; i--)
  {
    if (colours[by_f_time[i]] == white)
    {
      dfs_sc(rev, num_variables, by_f_time[i],
             colours, components, currentComponent);
      currentComponent += 1;
    }
  }
  *num_components = currentComponent;
  return components;
}

void reach_visit(Digraph &dig, const Vertex &v, std::vector<int> &d_to_u, std::vector<int> &preds, int current_d)
{
  std::for_each(boost::out_edges(v, dig).first,
                boost::out_edges(v, dig).second,
                [&](const auto &arc)
                {
                  auto targetV = boost::target(arc, dig);
                  if (d_to_u[targetV] < 0)
                  {
                    d_to_u[targetV] = current_d + 1;
                    preds[targetV] = v;
                    reach_visit(dig, targetV, d_to_u, preds, current_d + 1);
                  }
                });
}

void print_path(const Vertex &u, const Vertex &v, std::vector<int> &preds, int num_variables)
{
  std::string output("");
  int current = v;
  bool printed_first = false;
  while (current != -1)
  {
    int current_display = current + 1;
    if (current - num_variables >= 0)
    {
      current_display = (current - num_variables) * (-1) - 1;
    }
    // output = "(" + std::to_string(current) + ", " + std::to_string(current_display) + ", " + std::to_string(current - num_variables) + ") " + output;
    if (printed_first)
    {
      output = " " + output;
    }
    output = std::to_string(current_display) + output;
    current = preds[current];
    printed_first = true;
  }
  std::cout << output << "\n";
}

void build_and_print_path(const Vertex &u, const Vertex &v, int num_variables, Digraph &dig)
{
  std::vector<int> distance_to_u(num_variables * 2, -1);
  std::vector<int> preds(num_variables * 2, -1);
  distance_to_u[u] = 0;
  reach_visit(dig, u, distance_to_u, preds, 0);

  std::cout << distance_to_u[v] << " ";
  print_path(u, v, preds, num_variables);
}

Digraph condensed_digraph(std::vector<int> &components, int num_variables, int num_components, Digraph &dig)
{
  std::vector<std::pair<int, int>> edges;
  std::for_each(boost::vertices(dig).first,
                boost::vertices(dig).second,
                [&](const auto &vertex)
                {
                  std::for_each(boost::out_edges(vertex, dig).first,
                                boost::out_edges(vertex, dig).second,
                                [&](const auto &arc)
                                {
                                  // reverse condensed digraph
                                  auto compSource = components[vertex];
                                  auto compTarget = components[boost::target(arc, dig)];
                                  auto edge = std::make_pair(compSource, compTarget);
                                  // std::cout << "Make pair " << compSource << ", " << compTarget << "\n";
                                  if (compTarget != compSource)
                                  {

                                    edges.push_back(edge);
                                  }
                                });
                });
  // guarantees copy elision in c++17
  return Digraph(edges.cbegin(), edges.cend(), num_components);
}
void make_assignment(int component, std::vector<int> &components, int num_components, std::vector<int> &assign, int num_variables, Digraph &rev_condensed)
{
  // std::cout << "assignin on component " << component << "\n";
  bool has_assignment = false;
  // traverse all digraph do find vertices from this component and check wether they were already assigned
  for (int i = 0; i < num_variables; i++)
  {
    // if there is a variable with a literal in this component that already has been assigned
    if ((components[i] == component || components[i + num_variables] == component) && assign[i] != -1)
    {
      has_assignment = true;
      break;
    }
  }
  if (!has_assignment)
  {
    // mark literals from current component positive
    for (int i = 0; i < num_variables; i++)
    {
      if (components[i] == component)
      {
        // std::cout << "assignin 1 on var " << i + 1 << "\n";
        assign[i] = 1;
      }
    }
    for (int i = num_variables; i < num_variables * 2; i++)
    {
      if (components[i] == component)
      {
        // std::cout << "assignin 0 on var " << i + 1 - num_variables << "\n";
        assign[i - num_variables] = 0;
      }
    }
    // mark literals from opposite component negative ??
  }
  
}

void print_assignment(std::vector<int> &components, int num_variables, int num_components, Digraph &dig)
{
  Digraph condensed = condensed_digraph(components, num_variables, num_components, dig);
  std::vector<int> assign(num_variables, -1);

  for (int i = num_components - 1; i >= 0; i--)
  {
    make_assignment(i, components, num_components, assign, num_variables, condensed);
  }
  // print assign
  for (int i = 0; i < num_variables; i++)
  {
    std::cout << assign[i] << " ";
  }
}

int check_validity(std::vector<int> &components, int num_variables, int num_components, Digraph &dig)
{
  for (int i = 0; i <= num_variables; i++)
  {
    if (components[i] == components[i + num_variables])
    {
      int u, v;
      u = i;
      v = i + num_variables;
      std::cout << "NO"
                << "\n"
                << u + 1
                << "\n";
      build_and_print_path(u, v, num_variables, dig);
      build_and_print_path(v, u, num_variables, dig);
      return -1;
    }
  }
  std::cout << "YES"
            << "\n";

  print_assignment(components, num_variables, num_components, dig);
  return 1;
}

int main(int argc, char **argv)
{
  int debugLevel;
  std::cin >> debugLevel;
  if (debugLevel != 0)
  {
    // std::cout << "finish\n";
  }
  else
  {
    int num_variables, num_clauses, num_components;
    Digraph dig = read_digraph(std::cin, &num_variables, &num_clauses);
    HeadStart data = preprocess(dig, 0);

    Digraph rev = reverse_digraph(dig, num_variables * 2);
    std::vector<int> components = find_components(rev, num_variables, data.by_f_time, data.latest_f_time, &num_components);
    check_validity(components, num_variables, num_components, dig);
  }
  return EXIT_SUCCESS;
}
