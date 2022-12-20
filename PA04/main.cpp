#include <cstdlib> // for size_t, EXIT_SUCCESS, and EXIT_FAILURE

#include <exception>
#include <iostream>
#include <tuple> // for std::tie and std::ignore
#include <queue>
#include <stack>
#include <utility> // for pairs

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/optional.hpp>

#include "digraph.h"

using boost::optional;

using std::cout;
using std::endl;
using std::flush;

enum Color
{
    white,
    grey,
    black
};

// Digraph read_digraph(std::istream &is, typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m);

std::tuple<size_t, int, int, int> get_digraph_metadata(std::istream &is)
{
    int source;
    int target;
    size_t n;
    int m;

    is >> n;
    is >> m;

    is >> source;
    source -= 1;
    is >> target;
    target -= 1;

    std::cout << "Source: " << source + 1 << "\n";
    std::cout << "Target: " << target + 1 << "\n";

    return std::make_tuple(n, m, source, target);
}

std::vector<std::tuple<size_t, size_t, int>> get_digraph_arcs(std::istream &is, int m)
{
    std::vector<std::tuple<size_t, size_t, int>> arcs(0);

    while (m--)
    {
        int u, v;
        int capacity;
        is >> u >> v;
        is >> capacity;
        arcs.push_back(std::make_tuple(--u, --v, capacity));
        // std::cout << u + 1 << "->" << v + 1 << "(" << capacity << ")\n";
    }
    return arcs;
}

Digraph build_digraph(typename boost::graph_traits<Digraph>::vertices_size_type n, size_t m, std::vector<std::tuple<size_t, size_t, int>> arcs)
{
    Digraph digraph(n);
    for (auto const &arc : arcs)
    {
        int u, v, capacity;
        std::tie(u, v, capacity) = arc;
        Arc a;
        std::tie(a, std::ignore) = boost::add_edge(u, v, digraph);
        digraph[a].capacity = capacity;
        std::cout << u + 1 << "->" << v + 1 << "(" << capacity << ")\n";
    }

    return digraph;
}

Digraph compute_residual(Digraph &digraph)
{
    Digraph residual(boost::num_vertices(digraph));

    std::for_each(boost::edges(digraph).first,
                  boost::edges(digraph).second,
                  [&](const auto &arc)
                  {
                      auto sourceV = boost::source(arc, digraph);
                      auto targetV = boost::target(arc, digraph);

                      if (digraph[arc].capacity - digraph[arc].flow > 0)
                      {
                          Arc a;
                          std::tie(a, std::ignore) = boost::add_edge(sourceV, targetV, residual);
                          residual[a].direction = 1;
                          residual[a].capacity = digraph[arc].capacity;
                          residual[a].flow = digraph[arc].flow;
                          residual[a].res_capacity = digraph[arc].capacity - digraph[arc].flow;
                      }

                      if (digraph[arc].flow > 0)
                      {
                          Arc b;
                          std::tie(b, std::ignore) = boost::add_edge(targetV, sourceV, residual);
                          residual[b].direction = -1;
                          residual[b].capacity = digraph[arc].capacity;
                          residual[b].flow = digraph[arc].flow;
                          residual[b].res_capacity = digraph[arc].flow;
                      }
                  });
    return residual;
}

void print_residual_capacities(Digraph &digraph, std::vector<std::tuple<size_t, size_t, int>> arcs)
{
    // std::cout << "Residual capacities: \n";

    for (auto const &arc : arcs)
    {
        int u, v, capacity;
        std::tie(u, v, capacity) = arc;
        Arc a;
        std::tie(a, std::ignore) = boost::edge(u, v, digraph);
        // std::cout << u + 1 << "->" << v + 1 << " (" << 1 << ") = " << digraph[a].capacity - digraph[a].flow << " \n";
        // std::cout << u + 1 << "->" << v + 1 << " (" << -1 << ") = " << digraph[a].flow << " \n\n";
        std::cout << digraph[a].capacity - digraph[a].flow << " " << digraph[a].flow << " \n";
    }
}

std::pair<std::vector<int>, std::vector<int>> shortest_path(Digraph &residual, int source, int target)
{
    std::vector<int> dist(boost::num_vertices(residual), std::numeric_limits<int>::max());
    std::vector<int> pred(boost::num_vertices(residual));
    std::vector<int> color(boost::num_vertices(residual), Color::white); // 0 white, 1 grey, -1 black
    std::vector<int> shortest_path_v;

    dist[source] = 0;
    std::queue<int> queue;

    for (const auto &vertex : boost::make_iterator_range(boost::vertices(residual)))
    {
        pred[vertex] = vertex;
    }
    queue.push(source);

    while (!queue.empty())
    {
        auto u = queue.front();
        queue.pop();
        // std::cout << "vertex: " << u + 1 << ", dist = " << dist[u] << "\n";
        for (const auto &edge : boost::make_iterator_range(boost::out_edges(u, residual)))
        {
            auto v = boost::target(edge, residual);
            if (color[v] == Color::white)
            {
                color[v] = Color::grey;
                pred[v] = u * residual[edge].direction;
                dist[v] = dist[u] + 1;
                queue.push(v);
                // std::cout << "new edge: " << u + 1 << " to " << v + 1 << "\n";
            }
        }
        color[u] = Color::black;
    }
    // std::cout << "pred[target] = " << pred[target] << "\n";
    if (pred[target] != target)
    {
        // std::cout << "PATH from " << source + 1 << " to " << target + 1 << "\n";
        std::stack<int> path_stack;
        int pi = target;
        while (abs(pi) != source)
        {
            path_stack.push(pi);
            // std::cout << pi + 1 << " (" << (reversed ? "-1" : "1") << ") ";
            pi = pred[pi];
        }
        // std::cout << source + 1 << "\n";
        path_stack.push(pi);

        pi = path_stack.top();
        path_stack.pop();
        // std::cout << pi + 1;
        shortest_path_v.push_back(pi);
        while (!path_stack.empty())
        {
            pi = path_stack.top();
            path_stack.pop();
            shortest_path_v.push_back(pi);
            // bool reversed = pi != abs(pi);
            // std::cout << " (" << (reversed ? "-1" : "1") << ") " << pi + 1;
        }
        // std::cout << "\n";
    }
    return std::make_pair(shortest_path_v, color);
}

void print_shortest_path(Digraph &digraph, std::vector<std::tuple<size_t, size_t, int>> arcs, std::vector<int> path, int source, int target)
{
    std::cout << "PATH from " << source + 1 << " to " << target + 1 << "\n";
    int previous = source;
    int current = source;
    int epslon = std::numeric_limits<int>::max();
    int length = 0;
    std::vector<int> arcs_indexes;

    for (const auto vertex : path)
    {
        previous = current;
        current = vertex;
        if (vertex != source)
        {
            bool reversed = current != abs(current);
            int u, v, f;
            for (int i = 0; i < arcs.size(); i++)
            {
                std::tie(u, v, f) = arcs[i];

                // std::cout << u << ", " << v << ", " << f << "\n";

                if (u == abs(previous) && v == abs(current))
                {
                    // std::cout << i + 1 << " ";
                    arcs_indexes.push_back(i);
                    Arc a;
                    std::tie(a, std::ignore) = boost::edge(u, v, digraph);
                    int original_flow = digraph[a].flow;
                    int residual_cap = digraph[a].capacity - original_flow;
                    if (reversed)
                    {
                        residual_cap = original_flow;
                    }
                    epslon = epslon < residual_cap ? epslon : residual_cap;
                    length = length + 1;
                    break;
                }
            }
            // std::cout << " (" << (reversed ? "-1" : "1") << ") " << vertex + 1;
        }
    }
    std::cout << "0 " << epslon << " " << length << "\n";
    for (const auto index : arcs_indexes)
    {
        std::cout << index + 1 << " ";
    }
    std::cout << "\n";
}

int max_integral_flow(Digraph &digraph, std::vector<std::tuple<size_t, size_t, int>> arcs,
                      int source, int target)
{
    for (int t = 0; t <= 5; t++)
    {
        print_residual_capacities(digraph, arcs);
        Digraph residual = compute_residual(digraph);
        std::vector<int> shortest_path_res, colors;
        std::tie(shortest_path_res, colors) = shortest_path(residual, source, target);
        if (colors[target] == white)
        {
            std::cout << "NO PATH FOUND\n";
        }
        else
        {
            print_shortest_path(digraph, arcs, shortest_path_res, source, target);
        }
        //  ...
    }
    return 1;
}

int main(int argc, char **argv)
{
    try
    {

        size_t n;
        int m, source, target;
        std::tie(n, m, source, target) = get_digraph_metadata(std::cin);
        const std::vector<std::tuple<size_t, size_t, int>> arcs = get_digraph_arcs(std::cin, m);
        Digraph digraph{build_digraph(n, m, arcs)};
        max_integral_flow(digraph, arcs, source, target);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}