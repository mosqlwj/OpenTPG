//
// Created by luolijun on 2021/8/13.
//
#include "ATPGDriver.h"

#include "ATPGDriverDefault.h"
#include "ContextDefault.h"
#include "asserts.h"

extern ATPGDriver* CreateATPGDriver(void* context)
{
    ASSERT(context != nullptr);
    return new ATPGDriverDefault(reinterpret_cast<ContextDefault*>(context));
}
