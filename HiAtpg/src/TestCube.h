#ifndef HIATPG_TESTCUBE_H
#define HIATPG_TESTCUBE_H
#include "Common.h"
#include "Netlist.h"
#include <iostream>
#include <string>
#include <unistd.h>

class TestCube {
public:
    TestCube()
    {
    }

    ~TestCube()
    {
    }

    int Init(int PINum, int ScanCellNum, int32_t cycleNum, uint32_t faultId)
    {
        faultIndex = faultId;
        if (cycleNum <= 0) {
            perror("Defined cycle num must larger than 0");
            return -1;
        }
        logicValue.resize(cycleNum, std::vector<LogicVal> {});
        logicValue[0].resize(PINum + ScanCellNum, LogicVal::LOGIC_X);
        if (cycleNum <= 1) {
            return 0;
        }
        for (size_t i = 1; i < cycleNum; i++) {
            logicValue[i].resize(PINum, LogicVal::LOGIC_X);
        }
        return 0;
    }

    static TestCube* GenRandomCube(Netlist* netList, uint32_t faultId)
    {
        TestCube* randCube = new TestCube();
        int cycleNum = rand() % 3 + 1;

        printf("gen cycle num %d.\n", cycleNum);
        randCube->Init(
            netList->GetPICount(), netList->GetScanCellCount(), cycleNum, faultId);

        for (size_t cycleId = 0; cycleId < cycleNum; cycleId++) {
            for (size_t i = 0; i < netList->GetCubeEndId(); i++) {
                int randBit = rand() % 3;
                randCube->UpdateTestCubeValue(
                    cycleId, i, static_cast<LogicVal>(randBit));
            }
        }
        return randCube;
    }

    void UpdateTestCubeValue(int cycle, int index, LogicVal val)
    {
        logicValue[cycle][index] = val;
        return;
    }

    const std::vector<std::vector<LogicVal>>& GetLogicValue() const
    {
        return logicValue;
    }

    int GetCycleSize() const
    {
        return logicValue.size();
    }

    int GetFaultIndex() const
    {
        return faultIndex;
    }

private:
    uint32_t faultIndex;
    std::vector<std::vector<LogicVal>> logicValue;
};

#endif