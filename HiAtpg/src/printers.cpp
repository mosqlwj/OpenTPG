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
