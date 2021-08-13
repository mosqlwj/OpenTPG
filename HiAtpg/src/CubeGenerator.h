#ifndef CUBEGENERATOR_H
#define CUBEGENERATOR_H

struct Fault;
struct TestCube;

class CubeGenerator {
public:
    virtual ~CubeGenerator() = default;
    virtual TestCube* Generate(const Fault* fault) = 0;
};

extern CubeGenerator* CreateCubeGenerator(void* context);

#endif // CUBEGENERATOR_H
