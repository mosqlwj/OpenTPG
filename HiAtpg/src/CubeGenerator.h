#ifndef CUBEGENERATOR_H
#define CUBEGENERATOR_H

#include "common.h"

struct Fault;
struct TestCube;

class CubeGenerator {
public:
    virtual ~CubeGenerator() = default;
    virtual FaultStatus Generate(const Fault* fault, TestCube* cube) = 0;
};

extern CubeGenerator* CreateCubeGenerator();

#endif // CUBEGENERATOR_H
