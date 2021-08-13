//
// Created by luolijun on 2021/7/7.
//

#ifndef CUBEGENERATOREXAMPLE_H
#define CUBEGENERATOREXAMPLE_H

#include "CubeGenerator.h"

class Netlist;
struct ContextDefault;

class CubeGeneratorExample : public CubeGenerator {
public:
    CubeGeneratorExample(ContextDefault* context);
    ~CubeGeneratorExample() override;
    FaultStatus Generate(const Fault* fault, TestCube*& cube) override;

private:
    const ContextDefault* context;
};

#endif // CUBEGENERATOREXAMPLE_H
