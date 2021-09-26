//
// Created by luolijun on 2021/9/26.
//

#ifndef SIMENTRY_H
#define SIMENTRY_H

#include "Faultlist.h"
#include "SimFault.h"
#include "SimGood.h"
#include "TestCube.h"

class SimEntry {
public:
    SimEntry(Netlist* net, Faultlist* flist)
        : netlist(net)
    {
        goodSimulator = new SimGood(net);
        faultSimulator = new SimFault(net, fList);
    }
    ~SimEntry()
    {
        if (nullptr != goodSimulator) {
            delete goodSimulator;
            goodSimulator = nullptr;
        }
        if (nullptr != faultSimulator) {
            delete faultSimulator;
            faultSimulator = nullptr;
        }
    }
    bool HandleTestCube(TestCube* testCube);

private:
    SimGood* goodSimulator = nullptr;
    SimFault* faultSimulator = nullptr;
    Netlist* netlist = nullptr;
};

#endif //SIMENTRY_H
