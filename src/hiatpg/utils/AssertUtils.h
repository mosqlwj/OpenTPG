//
// Created by luolijun on 2020/7/16.
//

#ifndef OPENTPG_ASSERTUTILS_H
#define OPENTPG_ASSERTUTILS_H

#ifdef DEBUG
#ifndef ASSERT
#define ASSERT(expr) assert(expr)
#endif//ASSERT
#else
#define ASSERT(x)
#endif


#endif //OPENTPG_ASSERTUTILS_H
