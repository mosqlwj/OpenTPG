#ifndef HIATPG_CUBEOUTPUT_H
#define HIATPG_CUBEOUTPUT_H

#include "Netlist.h"
#include "TestCube.h"
#include <fstream>
#include <iostream>
#include <map>

typedef int32_t FaultId;
class CubeOutput
{
private:
    /* data */
public:
    static CubeOutput *GetOutputInstance()
    {
        static CubeOutput output;
        return &output;
    }

    void PrintCubes2File(std::map<FaultId, const TestCube *> &cubes)
    {
        std::ofstream stream("./cubes.ascii");
        if (!stream.is_open())
        {
            std::cout << "open cubes.ascii failed." << std::endl;
        }
        for (auto cubeInfo : cubes)
        {
            stream << "Fault Id: " << cubeInfo.first << "\n";
            int cycleSize = cubeInfo.second->GetCycleSize();

            const std::vector<std::vector<char>> &logicValue = cubeInfo.second->GetLogicValue();
            for (int cycle = 0; cycle < cycleSize; cycle++)
            {
                size_t valueSize = logicValue[cycle].size();
                for (int idx = 0; idx < valueSize; i++)
                {
                    stream << logicValue[cycle][idx];
                }
                stream << "\n";
            }
            stream.close();
        }
    }

private:
    CubeOutput(/* args */);
    ~CubeOutput();
};
s
#endif
