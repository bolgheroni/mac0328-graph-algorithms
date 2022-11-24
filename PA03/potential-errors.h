/* this file MUST NOT be edited by the students */

#ifndef POTENTIAL_ERRORS_H
#define POTENTIAL_ERRORS_H

#include <exception>
#include <sstream>
#include <string>

#include "potential.h"

class NotPotential_error : public std::exception
{
public:
  NotPotential_error(size_t real, size_t expected) {
    std::ostringstream oss;
    oss << "Attempt to construct a FeasiblePotential with a vector of size "
        << real << " when it should have size " << expected << '\n';
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

class InfeasiblePotential_error : public std::exception
{
public:
  InfeasiblePotential_error(const Digraph& digraph, const Arc& arc,
                            const std::vector<double> y) {
    const Vertex& u = source(arc, digraph);
    const Vertex& v = target(arc, digraph);
    double cost = digraph[arc].cost;
    std::ostringstream oss;
    oss << "Attempt to construct a FeasiblePotential, "
        << "but inequality y[v] <= y[u] + c(uv) is violated by arc (u,v) = ("
        << u+1 << "," << v+1 << ")\n"
        << "y[" << v+1 << "] = " << y[v] << '\n'
        << "y[" << u+1 << "] = " << y[u] << '\n'
        << "cost(" << u+1 << "," << v+1 << ") = " << cost << '\n';
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

class NotMultiplier_error : public std::exception
{
public:
  NotMultiplier_error(size_t real, size_t expected) {
    std::ostringstream oss;
    oss << "Attempt to construct a FeasibleMultiplier with a vector of size "
        << real << " when it should have size " << expected << '\n';
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

class InvalidMultiplier_error : public std::exception
{
public:
  InvalidMultiplier_error(size_t index, double value) {
    std::ostringstream oss;
    oss.precision(16);
    oss << "Attempt to construct a FeasibleMultiplier with a vector with a nonpositive entry: "
        << "z[" << index+1 << "] = " << value << '\n';
    _msg = oss.str();
  }
  virtual const char* what() const noexcept {
    return _msg.c_str();
  }
private:
  std::string _msg;
};

class InfeasibleMultiplier_error : public std::exception
{
public:
  InfeasibleMultiplier_error(const Digraph& digraph, const Arc& arc,
                             const std::vector<double> z) {
    const Vertex& u = source(arc, digraph);
    const Vertex& v = target(arc, digraph);
    double cost = digraph[arc].cost;
    std::ostringstream oss;
    oss << "Attempt to construct a FeasibleMultiplier, "
        << "but inequality z[v] >= z[u] * c(uv) is violated by arc (u,v) = ("
        << u+1 << "," << v+1 << ")\n"
        << "z[" << v+1 << "] = " << z[v] << '\n'
        << "z[" << u+1 << "] = " << z[u] << '\n'
        << "cost(" << u+1 << "," << v+1 << ") = " << cost << '\n';
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

#endif // #ifndef POTENTIAL_ERRORS_H
