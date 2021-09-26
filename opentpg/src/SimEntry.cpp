//
// Created by luolijun on 2021/9/26.
//

#include "SimEntry.h"

bool SimEntry::HandleTestCube(TestCube* testCube)
{
    goodSimulator->Init(testCube);
    goodSimulator->DoSim();
    faultSimulator->Init(testCube, goodSimulator);
    bool result = faultSimulator->DoSim();
    goodSimulator->Reset();
    faultSimulator->Reset();
    return result;
}
