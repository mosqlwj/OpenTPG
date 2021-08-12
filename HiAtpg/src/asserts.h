//
// Created by luolijun on 2021/8/12.
//

#ifndef ASSERTS_H
#define ASSERTS_H

#include <cassert>

#if defined(DEBUG) && !defined(NDEBUG)
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

#endif //ASSERTS_H
