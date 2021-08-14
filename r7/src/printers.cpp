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
#include "printers.h"

#include "TestCube.h"
#include "common.h"

#include <fstream>

std::ofstream& operator<<(std::ofstream& stream, const TestCube& cube)
{
    stream << "$: " << cube.GetFaultIndex() << "\n";
    int cycleSize = cube.GetCycleSize();

    const std::vector<std::vector<LogicVal>>& logicValue = cube.GetLogicValue();
    for (int cycle = 0; cycle < cycleSize; cycle++) {
        size_t valueSize = logicValue[cycle].size();
        for (int idx = 0; idx < valueSize; idx++) {
            stream << charOfLogicVal(logicValue[cycle][idx]);
        }
        stream << "\n";
    }

    return stream;
}

std::ofstream& operator<<(std::ofstream& stream, const Fault& fault)
{
    stream << fault.type << " " << charOfFaultStatus(fault.status) << " " << fault.pinName << std::endl;

    return stream;
}
