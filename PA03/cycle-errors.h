/* this file MUST NOT be edited by the students */

#ifndef CYCLE_ERRORS_H
#define CYCLE_ERRORS_H

#include <exception>
#include <sstream>
#include <string>

#include "cycle.h"

class NotCycle_error : public std::exception
{
public:
  NotCycle_error(const Walk& walk) {
    std::ostringstream oss;
    oss << "Attempt to construct a Cycle from the following non-cycle walk.\n"
        << walk;
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

class NonnegativeCycle_error : public std::exception
{
public:
  NonnegativeCycle_error(const Cycle& cycle, double cost) {
    std::ostringstream oss;
    oss.precision(16);
    oss << "Attempt to construct a NegativeCycle from a nonnegative cycle (of cost " << cost << ").\n"
        << cycle;
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

class NonLoophole_error : public std::exception
{
public:
  NonLoophole_error(const Cycle& cycle, double yield) {
    std::ostringstream oss;
    oss.precision(16);
    oss << "Attempt to construct a Loophole from a cycle with yield " << yield << " <= 1.0.\n"
        << cycle;
    _msg = oss.str();
  }
  virtual const char* what() const noexcept { return _msg.c_str(); }

private:
  std::string _msg;
};

#endif // #ifndef CYCLE_ERRORS_H
