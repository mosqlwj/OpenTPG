#ifndef CUBEGENERATOR_H
#define CUBEGENERATOR_H

struct Fault;
struct TestCube;
struct CubeGenerator {
  virtual ~CubeGenerator() = 0;
  virtual TestCube* Generate(const Fault* fault) = 0;
};

#endif  // CUBEGENERATOR_H
