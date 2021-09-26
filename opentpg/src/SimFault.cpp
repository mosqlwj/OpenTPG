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
#include "SimFault.h"

bool SimFault::DoSim()
{
    PrepareForSim(netlist);
    for (int cycleId = 0; cycleId < cycleNum; cycleId++) {
        DoEventDriven(cycleId);
        PrepareForSim(netlist);
    }
    return CheckObserved();
}

void SimFault::PrepareForSim(Netlist* nlist)
{
    ASSERT(nlist != nullptr);
    CleanQueue();
}

void SimFault::Init(TestCube* tCube, Fault* tFault, SimGood* goodSim)
{
    ASSERT(tCube != nullptr);
    ASSERT(tFault != nullptr);
    ASSERT(goodSim != nullptr);

    cycleNum = tCube->GetLogicValue().size();
    testCube = tCube;
    fault = tFault;
    goodSimulation = goodSim;
}

void SimFault::DoEventDriven(int32_t cycleId)
{
    if (cycleId != 0) {
        InitialStateEvent(cycleId);
    }
    InitialCyclePIInput(cycleId);
    TraceByLevel(cycleId);
    InitialFaultEvent(cycleId);
    TraceByLevel(cycleId);
}

void SimFault::InitialFaultEvent(int32_t cycleId)
{
    static LogicVal faultValueType[] = { LOGIC_0, LOGIC_1 };
    const Gate* faultFannoutGate = (fault->pin == 0) ? fault->gate : (fault->gate->inputs)[fault->pin - 1];
    if (fault->pin == 0) {
        if (fault->type == STUCK_AT_0 && goodSimulation->GetGoodValue(cycleId, fault->gate->id) == LOGIC_0) {
            return;
        }
        if (fault->type == STUCK_AT_1 && goodSimulation->GetGoodValue(cycleId, fault->gate->id) == LOGIC_1) {
            return;
        }
        faultMechine[cycleId][fault->gate->id] = faultValueType[fault->type];
    } else {
        faultMechine[cycleId][faultFannoutGate->id] = faultValueType[fault->type];
        SimGate(cycleId, const_cast<Gate*>(fault->gate));
        faultMechine[cycleId][faultFannoutGate->id] = goodSimulation->GetGoodValue(cycleId, faultFannoutGate->id);
        if (faultMechine[cycleId][fault->gate->id] == goodSimulation->GetGoodValue(cycleId, fault->gate->id)) {
            return;
        }
    }
    for (auto fanout : fault->gate->outputs) {
        AddQueue(fanout);
    }
}

void SimFault::TraceByLevel(int32_t cycleId)
{
    while (!eventQueue.empty()) {
        Gate* curGate = eventQueue.front();
        eventQueue.pop();
        SimGate(cycleId, curGate);
        if (curGate->type != DFF) {
            for (auto fanout : curGate->outputs) {
                AddQueue(fanout);
            }
        }
    }
}

bool SimFault::CheckObserved()
{
    const std::vector<std::vector<LogicVal>>& cubes = testCube->GetLogicValue();
    const std::vector<std::vector<LogicVal>>& goodMechine = goodSimulation->GetGoodMechine();

    // scan cell
    for (int i = netlist->GetPICount(); i < cubes[0].size(); i++) {
        if (faultMechine[cycleNum][i] == LOGIC_UNKNOW) {
            continue;
        }

        if (goodMechine[cycleNum][i] == LOGIC_X || faultMechine[cycleNum][i] == LOGIC_X) {
            continue;
        }

        if (goodMechine[cycleNum][i] != faultMechine[cycleNum][i]) {
            return true;
        }
    }

    // po
    for (int i = netlist->Gates().size() - netlist->GetPOCount(); i < netlist->Gates().size(); i++) {
        if (faultMechine[cycleNum - 1][i] == LOGIC_UNKNOW) {
            continue;
        }

        if (goodMechine[cycleNum - 1][i] == LOGIC_X || faultMechine[cycleNum - 1][i] == LOGIC_X) {
            continue;
        }

        if (goodMechine[cycleNum - 1][i] != faultMechine[cycleNum - 1][i]) {
            return true;
        }
    }

    return false;
}

void SimFault::Reset()
{
    faultMechine.resize(MAXCYCLENUM);
    for (int i = 0; i < MAXCYCLENUM; i++) {
        for (int j = 0; j < faultMechine[i].size(); j++) {
            faultMechine[i][j] = LOGIC_UNKNOW;
        }
    }
    for (int i = 0; i < MAXCYCLENUM; i++) {
        observableGates[i].clear();
    }
}

void SimFault::InitialStateEvent(int32_t cycleId)
{
    if (cycleId == 0) {
        return;
    }
    for (std::set<Gate*>::iterator ite = observableGates[cycleId - 1].begin();
         ite != observableGates[cycleId - 1].end(); ite++) {
        Gate* state = *ite;
        if (state->type != DFF) {
            continue;
        }
        for (auto fanout : state->outputs) {
            AddQueue(fanout);
        }
    }
}

void SimFault::InitialCyclePIInput(int32_t cycleId)
{
    const std::vector<Gate*>& gateVec = netlist->Gates();
    const std::vector<std::vector<LogicVal>>& logicValues = testCube->GetLogicValue();
    for (int i = 0; i < netlist->GetPICount(); i++) {
        faultMechine[cycleId][i] = logicValues[cycleId][i];
    }
}

void SimFault::SimGate(int32_t cycleId, Gate* curGate)
{
    switch (curGate->type) {
    case INV: {
        SimUtil::SimINV(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case BUF: {
        SimUtil::SimBUF(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case AND: {
        SimUtil::SimAND(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case NAND: {
        SimUtil::SimNAND(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case OR: {
        SimUtil::SimOR(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case NOR: {
        SimUtil::SimNOR(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case XOR: {
        SimUtil::SimNXOR(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case XNOR: {
        SimUtil::SimNXOR(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case MUX: {
        SimUtil::SimMUX(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    case DFF: {
        if (observableGates[cycleId].find(curGate) == observableGates[cycleId].end()) {
            observableGates[cycleId].insert(curGate);
        }
        // assume event only come from D
        if (SimUtil::ClockPulse(testCube, cycleId, curGate)) {
            faultMechine[cycleId + 1][curGate->id] = faultMechine[cycleId][SimUtil::GetStateDGate(curGate)->id];
        } else {
            faultMechine[cycleId + 1][curGate->id] = faultMechine[cycleId][curGate->id];
        }
        break;
    }
    case PI: {
        break;
    }
    case PO: {
        SimUtil::SimBUF(&(goodSimulation->GetGoodMechine()), &faultMechine, cycleId, curGate);
        break;
    }
    default: {
        break;
    }
    }
}
