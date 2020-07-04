//
// Created by fish on 2020/7/4.
//

#ifndef HIATPG_COMMON_H
#define HIATPG_COMMON_H

#include <vector>
#include <string>
#include <unordered_map>
#include <regex>
#include <iostream>

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

    Gate(GateId gateId, GateType type, const string& name)
        : gateId(gateId), type(type), name(name) {}
};

struct Fault {
    GateId gateId;
    int index;
    Value value;
    bool detected;

    Fault(GateId gateId, int index, Value value)
        : gateId(gateId), index(index), value(value), detected(false) {}
};

struct AtpgValue {
    Value goodVal;
    Value faultyVal;
};

class NetList {
private:
    vector<Gate*> gates;
    unordered_map<string, Gate*> name2GatePointer;

    static NetList* instance;

public:
    static NetList& GetInstance() {
        if (instance == nullptr) {
            instance = new NetList;
        }

        return *instance;
    }

    void Parse(string fileName) {
        cout << fileName << endl;
    }
};
NetList* NetList::instance = nullptr;

class FaultList {
    vector<Fault*> faults;
};

class AtpgEngine {
    unordered_map<GateId, AtpgValue> window;
    unordered_map<GateId, Value> testCube;
};

vector<string> testSplit11(const string& in, const string& delim)
{
    regex re{delim};
    return vector<string> {
            sregex_token_iterator(in.begin(), in.end(), re, -1),
            sregex_token_iterator()
    };
}

#endif //HIATPG_COMMON_H
