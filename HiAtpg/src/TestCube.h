/**
 * Copyright (c) [Year] [name of copyright holder]
 * [Software Name] is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#ifndef HIATPG_TESTCUBE_H
#define HIATPG_TESTCUBE_H

#include "Netlist.h"
#include "common.h"
#include <iostream>
#include <string>
#include <unistd.h>

struct TestCube {
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

    static TestCube* GenRandomCube(const Netlist* netList, uint32_t faultId)
    {
        TestCube* randCube = new TestCube();
        int cycleNum = rand() % 3 + 1;

        randCube->Init(netList->GetPICount(), netList->GetScanCellCount(), cycleNum, faultId);

        for (size_t cycleId = 0; cycleId < cycleNum; cycleId++) {
            int bitSize = (cycleId != 0) ? netList->GetPICount() : netList->GetPICount() + netList->GetScanCellCount();
            for (size_t i = 0; i < bitSize; i++) {
                int randBit = rand() % LogicVal::LOGIC_COUNT;
                randCube->UpdateTestCubeValue(cycleId, i, LogicVal(randBit));
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