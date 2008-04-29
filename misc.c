#include "misc.h"


#ifndef __GNUC__

bool __max_s(signed int a, signed int b) { return a > b ? a : b; }
bool __max_u(unsigned int a, unsigned int b)  { return a > b ? a : b; }
bool __min_s(signed int a, signed int b)  { return a > b ? b : a; }
bool __min_u(unsigned int a, unsigned int b)  { return a > b ? b : a; }


#endif
