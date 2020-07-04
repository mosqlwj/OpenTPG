//
// Created by fish on 2020/7/4.
//

#ifndef HIATPG_COMMON_H
#define HIATPG_COMMON_H

#include <vector>
#include <string>
#include <unordered_map>

using namespace std;
using GateId = int;

enum Value {
    X,
    ZERO,
    ONE,
    UNDEF
};

enum GateType {
    PI,
    PO,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR,
    INV,
    BUF
};

enum EventDir {
    FORWARD,
    BACKWORD,
    BOTH
};

struct Gate {
    GateId gateId;
    GateType type;
    vector<Gate*> fanins;
    vector<Gate*> fanouts;
    string name;
};

struct Fault {
    GateId gateId;
    int index;
    Value value;
    bool detected;
};

struct AtpgValue {
    Value goodVal;
    Value faultyVal;
};

class NetList {
    vector<Gate*> gates;
    unordered_map<string, Gate*> name2GatePointer;
};

class FaultList {
    vector<Fault*> faults;
};

class AtpgEngine {
    unordered_map<GateId, AtpgValue> window;
    unordered_map<GateId, Value> testCube;
};

#endif //HIATPG_COMMON_H
