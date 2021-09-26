//
// Created by luolijun on 2021/9/26.
//

#ifndef SIMFAULT_H
#define SIMFAULT_H

#include "Faultlist.h"
#include "Netlist.h"
#include "SimGood.h"
#include "TestCube.h"
#include <set>

class SimFault {
public:
    explicit SimFault(Netlist* net, Faultlist* flist)
        : netlist(net)
        , faultlist(flist)
    {
        observableGates.resize(MAXCYCLENUM);
        faultMechine.resize(MAXCYCLENUM);
        for (int i = 0; i < MAXCYCLENUM; i++) {
            faultMechine[i].resize(netlist->Gates().size(), LOGIC_UNKNOW);
        }
    }
    bool DoSim();
    void Init(TestCube* tCube, SimGood* goodSim);
    void Reset();

private:
    void PrepareForSim(Netlist* netlist);
    void DoEventDriven(int32_t cycleId);
    void InitialFaultEvent(int32_t cycleId);
    void TraceByLevel(int32_t cycleId);
    void SimGate(int32_t cycleId, Gate* curGate);
    bool CheckObserved();
    void InitialCyclePIInput(int32_t cycleId);
    void InitialStateEvent(int32_t cycleId);

private:
    inline void AddQueue(Gate* gate)
    {
        eventQueue.push(gate);
    }
    inline void CleanQueue()
    {
        while (!eventQueue.empty()) {
            eventQueue.pop();
        }
    }

private:
    int32_t cycleNum = 0;
    Netlist* netlist = nullptr;
    TestCube* testCube = nullptr;
    SimGood* goodSimulation = nullptr;
    Faultlist* faultlist = nullptr;
    std::vector<std::set<Gate*>> observableGates;
    std::vector<std::vector<LogicVal>> faultMechine;
    std::queue<Gate*> eventQueue;
};
#endif //SIMFAULT_H
