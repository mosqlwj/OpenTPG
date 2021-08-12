//
// Created by luolijun on 2021/7/7.
//

#ifndef CUBEGENERATOREXAMPLE_H
#define CUBEGENERATOREXAMPLE_H

#include "CubeGenerator.h"

class CubeGeneratorExample : public CubeGenerator {
public:
    CubeGeneratorExample();
    ~CubeGeneratorExample() override;
    TestCube* Generate(const Fault* fault) override;
};

#endif // CUBEGENERATOREXAMPLE_H
