#include "SimUtil.h"

uint64_t SimUtil::SimGate(Gate* gate, SimGM* goodMechine, uint64_t mask) {
}

uint64_t SimUtil::SimGate1(Gate* gate, SimGM* goodMechine, uint64_t mask) { 
    uint64_t res = 0;
    switch (gate->GetGateType()) { 
        case BUF:
            res = goodMechine[GetFaninGate(0)->GetGateId()];
            break;
        case INV:
            res = ~goodMechine[GetFaninGate(0)->GetGateId()];
        default:
            cout << "Can Not Sim Gate Which Have One Fanin!" << endl ;
    }
    goodMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimGate2(Gate* gate, SimGM* goodMechine, uint64_t mask) {
    uint64_t res = 0;
    // to be done
    switch (gate->GetGateType()) {
        default:
            cout << "Can Not Sim Gate Which Have Two Fanin!" << endl;
    }
    goodMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimGate3(Gate* gate, SimGM* goodMechine, uint64_t mask) {
    uint64_t res = 0;
    // to be done 
    switch (gate->GetGateType()) {
        case MUX:
            uint64_t fanin0 = goodMechine[gate->GetFaninGate(0)->GetGateId()];
            uint64_t fanin1 = goodMechine[gate->GetFaninGate(1)->GetGateId()];
            uint64_t fanin2 = goodMechine[gate->GetFaninGate(2)->GetGateId()];
            res = (fanin1 & ~fanin0) | (fanin2 & fanin0);
        default:
            cout << "Can Not Sim Gate Which Have Three Fanin!" << endl;
    }
    goodMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimGate4(Gate* gate, SimGM* goodMechine, uint64_t mask) { 
    uint64_t res = 0;
    uint64_t fanin0 = goodMechine[gate->GetFaninGate(0)->GetGateId()];
    uint64_t fanin1 = goodMechine[gate->GetFaninGate(1)->GetGateId()];
    uint64_t fanin2 = goodMechine[gate->GetFaninGate(2)->GetGateId()];
    uint64_t fanin3 = goodMechine[gate->GetFaninGate(2)->GetGateId()];
    switch (gate->GetGateType()) {
        case AND:
            res = fanin0 & fanin1 & fanin2 & fanin3;
            break;
        case NAND:
            res = ~(fanin0 & fanin1 & fanin2 & fanin3);
            break;
        case OR:
            res = fanin0 | fanin1 | fanin2 | fanin3;
            break;
        case NOR:
            res = ~(fanin0 | fanin1 | fanin2 | fanin3);
            break;
        case XOR:
            res = fanin0 ^ fanin1 ^ fanin2 ^ fanin3;
            break;
        case NXOR:
            res = ~(fanin0 ^ fanin1 ^ fanin2 ^ fanin3);
            break;
        default:
            cout << "Can Not Sim Gate Which Have Four Fanin!" << endl;
    }
    goodMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimFaultGate1(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine, 
    ValueManager* faultValueManager, uint64_t mask);
{ 
    uint64_t res = 0; 
    // to be done
    int32_t fanin0Id = gate->GetFaninGate(0)->GetGateId();
    uint64_t fanin0Val = faultMechine->Contains(fanin0Id) ? faultMechine[fanin0Id] : goodMechine[fanin0Id];
    switch (gate->GetGateType()) {
        case BUF:
            res = fanin0Val;            
            break;
        case INV:
            res = ~fanin0Val;            
        default:
            cout << "Can Not Sim Fault Gate Which Have One Fanin!" << endl;
    }
    faultMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimFaultGate2(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine,
    ValueManager* faultValueManager, uint64_t mask);
{
    uint64_t res = 0;
    // to be done
    switch (gate->GetGateType()) {

        default:
            cout << "Can Not Sim Fault Gate Which Have Two Fanin!" << endl;
    }
    faultMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimFaultGate3(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine,
    ValueManager* faultValueManager, uint64_t mask);
{
    uint64_t res = 0;
    // to be done
    int32_t fanin0Id = gate->GetFaninGate(0)->GetGateId();
    int32_t fanin1Id = gate->GetFaninGate(1)->GetGateId();
    int32_t fanin2Id = gate->GetFaninGate(2)->GetGateId();
    uint64_t fanin0Val = faultMechine->Contains(fanin0Id) ? faultMechine[fanin0Id] : goodMechine[fanin0Id];
    uint64_t fanin1Val = faultMechine->Contains(fanin1Id) ? faultMechine[fanin1Id] : goodMechine[fanin1Id];
    uint64_t fanin2Val = faultMechine->Contains(fanin2Id) ? faultMechine[fanin2Id] : goodMechine[fanin2Id];
    switch (gate->GetGateType()) {
        case MUX:
            res = (fanin1Val & ~fanin0Val) | (fanin2Val & fanin0Val);
            break;
        default:
            cout << "Can Not Sim Fault Gate Which Have Three Fanin!" << endl;
    }
    faultMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}

uint64_t SimUtil::SimFaultGate4(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine,
    ValueManager* faultValueManager, uint64_t mask);
{
    uint64_t res = 0;
    // to be done
    int32_t fanin0Id = gate->GetFaninGate(0)->GetGateId();
    int32_t fanin1Id = gate->GetFaninGate(1)->GetGateId();
    int32_t fanin2Id = gate->GetFaninGate(2)->GetGateId();
    int32_t fanin3Id = gate->GetFaninGate(3)->GetGateId();
    uint64_t fanin0Val = faultMechine->Contains(fanin0Id) ? faultMechine[fanin0Id] : goodMechine[fanin0Id];
    uint64_t fanin1Val = faultMechine->Contains(fanin1Id) ? faultMechine[fanin1Id] : goodMechine[fanin1Id];
    uint64_t fanin2Val = faultMechine->Contains(fanin2Id) ? faultMechine[fanin2Id] : goodMechine[fanin2Id];
    uint64_t fanin3Val = faultMechine->Contains(fanin3Id) ? faultMechine[fanin3Id] : goodMechine[fanin3Id];
    switch (gate->GetGateType()) {
        case AND:
            res = fanin0Val & fanin1Val & fanin2Val & fanin3Val;
            break;
        case NAND:
            res = ~(fanin0Val & fanin1Val & fanin2Val & fanin3Val);
            break;
        case OR:
            res = fanin0Val | fanin1Val | fanin2Val | fanin3Val;
            break;
        case NOR:
            res = ~(fanin0Val | fanin1Val | fanin2Val | fanin3Val);
            break;
        case XOR:
            res = fanin0Val ^ fanin1Val ^ fanin2Val ^ fanin3Val;
            break;
        case NXOR: 
            res = ~(fanin0Val ^ fanin1Val ^ fanin2Val ^ fanin3Val);
            break;
        default:
            cout << "Can Not Sim Fault Gate Which Have Four Fanin!" << endl;
    }
    faultMechine[gate->GetGateId()] = res & mask;
    return res & mask;
}