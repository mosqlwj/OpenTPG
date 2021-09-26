/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#ifndef SIMFAULT_H
#define SIMFAULT_H

#include "Faultlist.h"
#include "Netlist.h"
#include "SimGood.h"
#include "TestCube.h"
#include <set>

class SimFault {
public:
    SimFault()

    {
        observableGates.resize(MAXCYCLENUM);
        faultMechine.resize(MAXCYCLENUM);
    }

    void SetupNetlist(Netlist* nlist)
    {
        ASSERT(nlist != nullptr);

        netlist = nlist;
        for (int i = 0; i < faultMechine.size(); i++) {
            faultMechine[i].resize(netlist->Gates().size(), LOGIC_UNKNOW);
        }
    }

    void SetupFaultlist(Faultlist* flist)
    {
        ASSERT(flist != nullptr);

        faultlist = flist;
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
