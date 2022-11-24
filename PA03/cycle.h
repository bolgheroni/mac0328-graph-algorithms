/* this file MUST NOT be edited by the students */

#ifndef CYCLE_H
#define CYCLE_H

#include <functional>           // for wrapper_reference
#include <iostream>
#include <vector>

#include "digraph.h"

class Walk {
public:
  /* start building a walk in digraph from vertex start */
  Walk(const Digraph& digraph, const Vertex& start);

  /* extend the walk by adding arc to its end */
  bool extend(const Arc& arc);

  /* accessors */
  const Digraph& digraph() const { return _digraph.get(); }
  bool is_cycle() const { return _cycle; }
  const std::vector<Arc>& arcs() const { return _arcs; }

  friend std::ostream& operator<<(std::ostream& os, const Walk& walk);

private:
  std::reference_wrapper<const Digraph> _digraph;
  const Vertex _start;
  Vertex _last;
  std::vector<Arc> _arcs;
  bool _path, _cycle;
  std::vector<bool> _visited;
};

class Cycle
{
public:
  /* construction succeeds only when the parameter is really a cycle;
     otherwise it throws a NotCycle_error */
  Cycle(const Walk& cycle);

  /* accessors */
  const Digraph& digraph() const { return _digraph.get(); }
  const std::vector<Arc>& get() const { return _arcs; }

  friend std::ostream& operator<<(std::ostream& os, const Cycle& cycle);

protected:
  std::reference_wrapper<const Digraph> _digraph;
  std::vector<Arc> _arcs;
};

class NegativeCycle : public Cycle
{
public:
  /* construction succeeds only when the parameter is really a cycle
   * (otherwise a NotCycle_error is thrown), and its cost is
   * negative (otherwise a NonnegativeCycle_error is thrown). */
  NegativeCycle(const Walk& cycle);

  double cost() const { return _cost; }
private:
  double _cost;
};

class Loophole : public Cycle
{
public:
  /* construction succeeds only when the parameter is really a cycle
   * (otherwise a NotCycle_error is thrown), and its yield is
   * greater than 1 (otherwise a NonLoophole_error is thrown). */
  Loophole(const Walk& cycle);

  double yield() const { return _yield; }
private:
  double _yield;
};

#endif // #ifndef CYCLE_H
