//
// Created by fish on 2020/7/5.
//

#ifndef HIATPG_ATPGENGINE_H
#define HIATPG_ATPGENGINE_H

#include <queue>

#include "LogicValue.h"
#include "model.h"

struct TpgEvent {
    Gate* gate;
    EventDir dir;
    Value value;
    MachineType machineType;

    TpgEvent(Gate* gate, EventDir dir, Value value, MachineType machineType)
        : gate(gate), dir(dir), value(value), machineType(machineType) {}
};

class AtpgEngine {
private:
    unordered_map<GateId, AtpgValue> window;
    unordered_map<GateId, Value> testCube;
    queue<TpgEvent> tpgEventQueue;

public:
    bool ActivateFaultEffect(Fault* fault) {}

    bool ImplyGate(Gate* gate, Value value, MachineType machineType) {
        tpgEventQueue.push(move(TpgEvent(gate, BOTH, value, machineType)));
    }

    bool DoImplication() {
        while (!tpgEventQueue.empty()) {
            TpgEvent event = tpgEventQueue.front();
            tpgEventQueue.pop();
            Gate* curGate = event.gate;
            EventDir dir = event.dir;
            Value assignVal = event.value;
            MachineType machineType = event.machineType;

            Value curVal = GetCurVal(curGate->gateId, machineType);
            Value combineVal = CombineVal(curVal, assignVal);
            if (combineVal != UNDEF) {
                Value simVal = combineVal = CombineVal(simVal, combineVal);
            }

            if (combineVal != UNDEF) {
                bool needChange = combineVal == curVal ? true : false;
            }
        }
    }

    Value GetCurVal(GateId gateId, MachineType machineType) {
        Value value = X;
        if (machineType == GOODMACHINE) {
            value = window[gateId].goodVal;
        } else {
            value = window[gateId].faultyVal;
        }
        return value;
    }

    Value CombineVal(Value origin, Value dest) {
        if (origin == X) {
            return dest;
        }

        if (origin != dest) {
            return UNDEF;
        }

        return dest;
    }

    Value SimulateGate(Gate* gate, MachineType machineType) {
        uint8_t index = 0;
        int i = 0;
        for (auto fanin : gate->fanins) {
            index += GetCurVal(fanin->gateId, machineType) << i;
        }

        return
    }

    Value LookupValueTable(GateType type, int index) {
        Value value;
        switch (type) {
            case AND:
                value = simAnd[index];
                break;
            case NAND:
                value = simNand[index];
                break;
            case OR:
                value = simOr[index];
                break;
            case NOR:
                value = simNor[index];
                break;
            case XOR:
                value = simXor[index];
                break;
            case XNOR:
                value = simXnor[index];
                break;
            case INV:
                value = simInv[index];
                break;
            default:
                value = simBuf[index];
                break;
        }

        return value;
    }
};

#endif  // HIATPG_ATPGENGINE_H
