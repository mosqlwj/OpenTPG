//
// Created by luolijun on 2021/8/12.
//

#ifndef CUBEHANDLER_H
#define CUBEHANDLER_H

#include "TestCube.h"

class CubeHandler {
public:
    virtual ~CubeHandler() = default;
    virtual void Handle(TestCube* cube) = 0;
};

#endif //CUBEHANDLER_H
