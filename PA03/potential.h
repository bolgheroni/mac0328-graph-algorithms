#ifndef POTENTIAL_H
#define POTENTIAL_H

#include <functional>           // for wrapper_reference
#include <iostream>
#include <vector>

#include "digraph.h"

class FeasiblePotential
{
public:
  FeasiblePotential(const Digraph& digraph,
                    const std::vector<double> y);

  /* accessors */
  const Digraph& digraph() const { return _digraph.get(); }
  const std::vector<double>& potential() const { return _y; }

  friend std::ostream& operator<<(std::ostream& os,
                                  const FeasiblePotential& feaspot);

private:
  std::reference_wrapper<const Digraph> _digraph;
  std::vector<double> _y;
};

class FeasibleMultiplier
{
public:
  FeasibleMultiplier(const Digraph& digraph,
                     const std::vector<double> z);

  /* accessors */
  const Digraph& digraph() const { return _digraph.get(); }
  const std::vector<double>& multiplier() const { return _z; }

  friend std::ostream& operator<<(std::ostream& os,
                                  const FeasibleMultiplier& feasmult);

private:
  std::reference_wrapper<const Digraph> _digraph;
  std::vector<double> _z;
};

#endif // #ifndef POTENTIAL_H
