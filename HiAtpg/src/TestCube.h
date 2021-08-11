#ifndef HIATPG_TESTCUBE_H
#define HIATPG_TESTCUBE_H
#include "Netlist.h"
#include <iostream>

class TestCube
{
public:
  TestCube(Netlist* netList, int32_t cycleNum)
  {
    logicValue.resize(3, std::vector<char>(100, 'x'));
  }

  ~TestCube() {}

  int Init(int32_t faultId, int32_t size, int16_t cycleNum) { return 0; }

  void UpdateTestCubeValue(int cycle, int index, char val)
  {
    logicValue[cycle][index] = val;
    return;
  }

  const std::vector<std::vector<char>>& GetLogicValue() const
  {
    return logicValue;
  }

  int GetCycleSize() const { return logicValue.size(); }

private:
  std::vector<std::vector<char>> logicValue;
};

#endif