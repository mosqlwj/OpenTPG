#ifndef HIATPG_CUBEOUTPUT_H
#define HIATPG_CUBEOUTPUT_H

#include "Common.h"
#include "Netlist.h"
#include "Params.h"
#include "TestCube.h"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
typedef int32_t FaultId;
class CubeOutput
{
private:
  std::list<TestCube*> cubes;

public:
  static CubeOutput* GetOutputInstance()
  {
    static CubeOutput output;
    return &output;
  }

  int Commit(TestCube* c)
  {
    cubes.push_back(c);
    return 0;
  }

  void PrintCubes2File()
  {
    const std::string filepath = Params::GetInstance()->GetCubeDumpFile();

    std::ofstream stream(filepath);

    if (!stream.is_open()) {
      printf("open  %s failed.\n", filepath.c_str());
      return;
    }

    for (auto cubeInfo : cubes) {
      stream << "$: " << cubeInfo->GetFaultIndex() << "\n";
      int cycleSize = cubeInfo->GetCycleSize();

      const std::vector<std::vector<LogicVal>>& logicValue =
        cubeInfo->GetLogicValue();
      for (int cycle = 0; cycle < cycleSize; cycle++) {
        size_t valueSize = logicValue[cycle].size();
        for (int idx = 0; idx < valueSize; idx++) {
          stream << charOfLogicVal(logicValue[cycle][idx]);
        }
        stream << "\n";
      }
    }
    stream.close();
    printf("Save cube to %s successful.\n", filepath.c_str());
  }

private:
  CubeOutput(/* args */);
  ~CubeOutput();
};
#endif
