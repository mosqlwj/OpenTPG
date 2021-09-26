//
// Created by luolijun on 2021/9/26.
//

#ifndef SIMGOOD_H
#define SIMGOOD_H

#include "Netlist.h"
#include "SimUtil.h"
#include "TestCube.h"
#include "common.h"
#include <new>
#include <queue>
#include <set>

class SimGood {
public:
    explicit SimGood(Netlist* net)
        : netlist(net)
    {
        observableGates.resize(MAXCYCLENUM);
        goodMechine.resize(MAXCYCLENUM);
        for (int i = 0; i < MAXCYCLENUM; i++) {
            goodMechine[i].resize(netlist->Gates().size(), LOGIC_UNKNOW);
        }
    }
    void DoSim();
    void Reset();
    void Init(TestCube* tCube)
    {
        testCube = tCube;
        cycleNum = tCube->GetLogicValue().size();
    }
    LogicVal GetGoodValue(int32_t cycleId, GateId id)
    {
        return goodMechine[cycleId][id];
    }
    inline std::vector<std::vector<LogicVal>>& GetGoodMechine()
    {
        return goodMechine;
    }

private:
    void PrepareForSim(Netlist* netlist);
    void DoEventDriven(int32_t cycleId);
    void TraceByLevel(int32_t cycleId);
    void InitialPiEvent(int32_t cycleId);
    void InitialStateEvent(int32_t cycleId);
    void InitialFirstFrameStateEvent();
    bool InputReady(Gate* gate, int32_t cycleId);

    inline void AddQueue(Gate* gate)
    {
        if (valueManager->Contains(gate->id)) {
            return;
        }
        valueManager->Set(gate->id);
        eventQueue.emplace(gate);
    }

    inline void CleanQueue()
    {
        while (!eventQueue.empty()) {
            eventQueue.pop();
        }
    }

    inline void ResetValueManager(int32_t size)
    {
        if (nullptr == valueManager) {
            valueManager = new ValueManager();
            valueManager->Init(size);
        }
        valueManager->Reset();
    }

private:
    int cycleNum = 0;
    Netlist* netlist = nullptr;
    ValueManager* valueManager = nullptr;
    TestCube* testCube = nullptr;
    std::vector<std::vector<LogicVal>> goodMechine;
    std::vector<std::set<Gate*>> observableGates;
    std::queue<Gate*> eventQueue;
};

#endif //SIMGOOD_H
