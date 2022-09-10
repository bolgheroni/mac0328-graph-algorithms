/* this file MUST NOT be edited by the students */

#ifndef ASGT_H
#define ASGT_H

#include <iostream>
#include "arb.h"

bool is_ancestor (const Vertex& u, const Vertex& v, const HeadStart& data);
HeadStart preprocess (Arb &arb, const Vertex& root);
Arb read_arb(std::istream& in);

#endif // #ifndef ASGT_H
