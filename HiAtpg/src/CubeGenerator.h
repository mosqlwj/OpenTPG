#ifndef CUBEGENERATOR_H
#define CUBEGENERATOR_H

struct Fault;
struct TestCube;

struct CubeGenerator {
    virtual ~CubeGenerator() {};
    virtual TestCube* Generate(const Fault* fault) = 0;
};

extern CubeGenerator* CreateCubeGenerator();

#endif // CUBEGENERATOR_H
