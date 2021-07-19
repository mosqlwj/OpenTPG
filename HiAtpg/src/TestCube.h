#ifndef HIATPG_TESTCUBE_H
#define HIATPG_TESTCUBE_H
#include "Netlist.h"

class TestCube
{
private:
    std::vector<std::vector<char>> logicValue;

public:
    TestCube(Netlist *netList, int32_t cycleNum)
    {
    }
    ~TestCube();

    void UpdateTestCubeValue(int cycle, int index, char val)
    {
        logicValue[cycle][index] = val;
    }

    const std::vector<std::vector<char>> &GetLogicValue() const
    {
        return logicValue;
    }

    int GetCycleSize() const
    {
        return logicValue.size();
    }
};

#endif