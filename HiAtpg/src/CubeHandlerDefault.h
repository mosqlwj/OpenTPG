//
// Created by luolijun on 2021/8/12.
//
#ifndef CUBEOUTPUTDEFAULT_H
#define CUBEOUTPUTDEFAULT_H

#include "CubeHandler.h"
#include "asserts.h"

class CubeHandlerDefault : public CubeHandler {
public:
    virtual ~CubeHandlerDefault() = default;

    void Handle(TestCube* cube) override
    {
        ASSERT(cube != nullptr);
        delete cube;
    }
    
    void Commit() override
    {
    }
};

#endif //CUBEOUTPUTDEFAULT_H
