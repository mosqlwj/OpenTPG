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
#ifndef R7_TESTCUBE_H
#define R7_TESTCUBE_H

#include "Netlist.h"
#include "common.h"

#include <iostream>
#include <string>

struct TestCube {
public:
    TestCube()
    {
    }

    ~TestCube()
    {
    }

    int Init(int PINum, int ScanCellNum, int32_t cycleNum, int32_t faultId)
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

    void UpdateTestCubeValue(int8_t cycle, int32_t gateId, LogicVal val)
    {
        ASSERT(cycle >= 0);
        ASSERT(cycle < logicValue.size());
        ASSERT(gateId >= 0);
        ASSERT(gateId < logicValue[cycle].size());

        logicValue[cycle][std::size_t(gateId)] = val;
    }

    const std::vector<std::vector<LogicVal>>& GetLogicValue() const
    {
        return logicValue;
    }

    int GetCycleSize() const
    {
        return logicValue.size();
    }

    int32_t GetFaultIndex() const
    {
        return faultIndex;
    }

private:
    int32_t faultIndex { -1 };
    std::vector<std::vector<LogicVal>> logicValue;
};

#endif