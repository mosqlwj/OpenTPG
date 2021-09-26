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
#include "SimGood.h"
#include "SimUtil.h"

void SimGood::DoSim()
{
    PrepareForSim(netlist);
    for (int cycleId = 0; cycleId < cycleNum; cycleId--) {
        DoEventDriven(cycleId);
        PrepareForSim(netlist);
    }
}

void SimGood::PrepareForSim(Netlist* netlist)
{
    CleanQueue();
    ResetValueManager(netlist->Gates().size());
}

void SimGood::DoEventDriven(int32_t cycleId)
{
    InitialPiEvent(cycleId);
    if (cycleId == 0) {
        InitialFirstFrameStateEvent();
    } else {
        InitialStateEvent(cycleId);
    }
    TraceByLevel(cycleId);
}

void SimGood::TraceByLevel(int32_t cycleId)
{
    while (!eventQueue.empty()) {
        Gate* curGate = eventQueue.front();
        eventQueue.pop();
        if (!InputReady(curGate, cycleId)) {
            eventQueue.push(curGate);
            continue;
        }
        switch (curGate->type) {
        case INV: {
            SimUtil::SimINV(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case BUF: {
            SimUtil::SimBUF(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case AND: {
            SimUtil::SimAND(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case NAND: {
            SimUtil::SimNAND(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case OR: {
            SimUtil::SimOR(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case NOR: {
            SimUtil::SimNOR(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case XOR: {
            SimUtil::SimNXOR(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case XNOR: {
            SimUtil::SimNXOR(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case MUX: {
            SimUtil::SimMUX(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        case DFF: {
            if (observableGates[cycleId].find(curGate) == observableGates[cycleId].end()) {
                observableGates[cycleId].insert(curGate);
            }
            // assume event only come from D
            if (SimUtil::ClockPulse(testCube, cycleId, curGate)) {
                goodMechine[cycleId + 1][curGate->id] = goodMechine[cycleId][SimUtil::GetStateDGate(curGate)->id];
            } else {
                goodMechine[cycleId + 1][curGate->id] = goodMechine[cycleId][curGate->id];
            }
            break;
        }
        case PI: {
            break;
        }
        case PO: {
            SimUtil::SimBUF(&goodMechine, nullptr, cycleId, curGate);
            break;
        }
        default: {
            break;
        }
        }
        if (curGate->type != DFF) {
            for (auto fanout : curGate->outputs) {
                AddQueue(fanout);
            }
        }
    }
}

void SimGood::InitialPiEvent(int32_t cycleId)
{
    const std::vector<Gate*>& gateVec = netlist->Gates();
    const std::vector<std::vector<LogicVal>>& logicValues = testCube->GetLogicValue();
    for (int i = 0; i < netlist->GetPICount(); i++) {
        goodMechine[cycleId][i] = logicValues[cycleId][i];
        const Gate* curGate = gateVec[i];
        for (auto fanout : curGate->outputs) {
            AddQueue(fanout);
        }
    }
}

void SimGood::InitialStateEvent(int32_t cycleId)
{
    if (cycleId == 0) {
        return;
    }
    for (std::set<Gate*>::iterator ite = observableGates[cycleId - 1].begin();
         ite != observableGates[cycleId - 1].end(); ite++) {
        Gate* state = *ite;
        if (state->type != DFF) {
            continue;
            ;
        }
        for (auto fanout : state->outputs) {
            AddQueue(fanout);
        }
    }
}

void SimGood::InitialFirstFrameStateEvent()
{
    const std::vector<Gate*>& gateVec = netlist->Gates();
    const std::vector<std::vector<LogicVal>>& logicValues = testCube->GetLogicValue();
    // initial scan-cell
    int scanCellIndex = netlist->GetPICount();
    for (; scanCellIndex < logicValues[0].size(); scanCellIndex++) {
        goodMechine[0][scanCellIndex] = logicValues[0][scanCellIndex];
        Gate* curGate = gateVec[scanCellIndex];
        for (auto fanout : curGate->outputs) {
            AddQueue(fanout);
        }
    }
    // initial non-scan cell
    for (; scanCellIndex < (netlist->GetPICount() + netlist->GetDffCount()); scanCellIndex++) {
        goodMechine[0][scanCellIndex] = LOGIC_X;
        Gate* curGate = gateVec[scanCellIndex];
        for (auto fanout : curGate->outputs) {
            AddQueue(fanout);
        }
    }
}

bool SimGood::InputReady(Gate* gate, int32_t cycleId)
{
    for (auto fanin : gate->inputs) {
        if (goodMechine[cycleId][fanin->id] == LOGIC_UNKNOW) {
            return false;
        }
    }
    return true;
}

void SimGood::Reset()
{
    goodMechine.resize(MAXCYCLENUM);
    for (int i = 0; i < MAXCYCLENUM; i++) {
        for (int j = 0; j < goodMechine[i].size(); j++) {
            goodMechine[i][j] = LOGIC_UNKNOW;
        }
    }
    for (int i = 0; i < MAXCYCLENUM; i++) {
        observableGates[i].clear();
    }
}
