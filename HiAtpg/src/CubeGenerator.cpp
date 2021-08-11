//
// Created by luolijun on 2021/8/11.
//
#include "CubeGenerator.h"
#include "CubeGeneratorExample.h"

extern CubeGenerator* CreateCubeGenerator()
{
    return new CubeGeneratorExample();
}
