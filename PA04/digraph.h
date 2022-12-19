#ifndef DIGRAPH_H
#define DIGRAPH_H

#define BOOST_ALLOW_DEPRECATED_HEADERS // silence warnings
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

struct BundledVertex
{
    size_t pi;
};

struct BundledArc
{
    double cost;
    BundledArc() : cost(0.0) {}
};

typedef boost::adjacency_list<boost::vecS,
                              boost::vecS,
                              boost::directedS,
                              BundledVertex,
                              BundledArc>
    Digraph;
typedef boost::graph_traits<Digraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<Digraph>::edge_descriptor Arc;

#endif // #ifndef DIGRAPH_H
