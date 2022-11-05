/* This is the driver. Code similar to this will be used for grading.
 * It may be edited by the students for testing purposes, however it
 * WILL NOT be submitted as part of the assignment.
 */

#include <iostream>
#include "asgt.h"

int main (int argc, char** argv)
{
  Arb arb = read_arb(std::cin);
  HeadStart data = preprocess (arb, 0);

  size_t queries; std::cin >> queries;

  while(queries--) {
    Vertex x, y; std::cin >> x >> y;
    std::cout << (is_ancestor (--x, --y, data) ? "YES" : "NO")
              << std::endl;
  }

  return EXIT_SUCCESS;
}
