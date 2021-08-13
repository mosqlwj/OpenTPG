//
// Created by luolijun on 2021/7/7.
//

#ifndef CUBEGENERATOREXAMPLE_H
#define CUBEGENERATOREXAMPLE_H

#include "CubeGenerator.h"
#include "common.h"

class CubeGeneratorExample : public CubeGenerator {
public:
    CubeGeneratorExample();
    ~CubeGeneratorExample() override;
    FaultStatus Generate(const Fault* fault, TestCube* cube) override;
};

#endif // CUBEGENERATOREXAMPLE_H
