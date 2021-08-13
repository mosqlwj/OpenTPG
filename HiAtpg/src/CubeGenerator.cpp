//
// Created by luolijun on 2021/8/11.
//
#include "CubeGenerator.h"
#include "CubeGeneratorExample.h"

#include "ContextDefault.h"
#include "asserts.h"

extern CubeGenerator* CreateCubeGenerator(void* context)
{
    ASSERT(context != nullptr);
    return new CubeGeneratorExample(reinterpret_cast<ContextDefault*>(context));
}
