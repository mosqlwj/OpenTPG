//
// Created by luolijun on 2021/8/13.
//
#include "ATPGDriver.h"

#include "ATPGDriverDefault.h"

extern ATPGDriver* CreateATPGDriver()
{
    return new ATPGDriverDefault;
}
