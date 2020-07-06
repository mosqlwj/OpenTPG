//
// Created by fish on 2020/7/4.
//

#ifndef HIATPG_MODEL_H
#define HIATPG_MODEL_H

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>

#include "StringUtils.h"

using namespace std;

using GateId = int;


enum Value { X, ZERO, ONE, UNDEF };

enum GateType { PI, PO, AND, NAND, OR, NOR, XOR, XNOR, INV, BUF };

unordered_map<string, GateType> str2GateType = {{"AND", AND}, {"NAND", NAND}, {"OR", OR},   {"NOR", NOR},
                                                {"XOR", XOR}, {"XNOR", XNOR}, {"INV", INV}, {"BUF", BUF}};

unordered_map<GateType, string> gateType2Str = {{AND, "AND"}, {NAND, "NAND"}, {OR, "OR"},   {NOR, "NOR"},
                                                {XOR, "XOR"}, {XNOR, "XNOR"}, {INV, "INV"}, {BUF, "BUF"}};
enum EventDir { FORWARD, BACKWORD, BOTH };

struct Options {};


struct Gate {
    GateId gateId;
    GateType type;
    vector<Gate*> fanins;
    vector<Gate*> fanouts;
    string name;

    Gate(GateId gateId, GateType type, const string& name) : gateId(gateId), type(type), name(name) {}
};

struct Fault {
    GateId gateId;
    int index;
    Value value;
    bool detected;

    Fault(GateId gateId, int index, Value value) : gateId(gateId), index(index), value(value), detected(false) {}
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
    static NetList* GetInstance() {
        if (instance == nullptr) {
            instance = new NetList;
        }

        return instance;
    }

    void Parse(string fileName) {
        ifstream in(fileName);
        GateId curGateId = 0;
        unordered_map<string, Gate*> primaryOutput;
        string line;

        while (getline(in, line)) {
            if (line.length() == 0) {
                continue;
            }

            StringUtils::trim(line);

            vector<string> paras;
            paras = StringUtils::split(line, "[=,()]");
            Gate* gate;

            if (paras[0] == "INPUT") {
                auto name = paras[1];
                gate = new Gate(curGateId, PI, name);
                name2GatePointer[name] = gate;
            } else if (paras[0] == "OUTPUT") {
                auto name = paras[1];
                gate = new Gate(curGateId, PO, name);
                primaryOutput[name] = gate;
            } else {
                auto name = paras[0];
                GateType type = str2GateType[paras[1]];
                gate = new Gate(curGateId, type, name);
                name2GatePointer[name] = gate;
                // add fanins
                for (int i = 2; i < paras.size(); ++i) {
                    auto fanin = name2GatePointer[paras[i]];
                    gate->fanins.push_back(fanin);
                    fanin->fanouts.push_back(gate);
                }
            }
            gates.push_back(gate);
            curGateId++;
        }

        for (auto& po : primaryOutput) {
            auto name = po.first;
            auto gate = po.second;
            Gate* fanin = name2GatePointer[name];
            gate->fanins.push_back(fanin);
            fanin->fanouts.push_back(gate);
        }
        for (auto& po : primaryOutput) {
            auto name = po.first;
            auto gate = po.second;
            gate->name = name + "_PO";
            name2GatePointer[gate->name] = gate;
        }

        return;
    }

    void PrintGates() {
        for (auto gate : gates) {
            cout << "GateId: " << gate->gateId << endl;
            cout << "GateName: " << gate->name << endl;
            cout << "GateType: " << gate->type << endl;

            for (auto fanin : gate->fanins) {
                cout << "Fanin: " << fanin->name << endl;
            }
            for (auto fanout : gate->fanouts) {
                cout << "Fanout: " << fanout->name << endl;
            }
            cout << endl;
        }
    }

    Gate* FindGateByName(string name) { return name2GatePointer[name]; }

    vector<Gate*>& GetGates() { return gates; }

    void ClearGates() {
        gates.clear();
        name2GatePointer.clear();
    }
};
NetList* NetList::instance = nullptr;

class FaultList {
private:
    vector<Fault*> faults;

    static FaultList* instance;

public:
    static FaultList* GetInstance() {
        if (instance == nullptr) {
            instance = new FaultList;
        }

        return instance;
    }

    void Parse(string fileName) {
        ifstream in(fileName);
        int curFaultIndex = 0;
        string line;

        while (getline(in, line)) {
            if (line.length() == 0) {
                continue;
            }

            vector<string> paras;
            paras = StringUtils::split(line, " ");
            auto netlist = NetList::GetInstance();
            auto gateName = paras[0];
            Gate* gate = netlist->FindGateByName(gateName);
            Value value = (Value)(stoi(paras[1]) + ZERO);
            Fault* fault = new Fault(gate->gateId, curFaultIndex, value);
            faults.push_back(fault);
            curFaultIndex++;
        }

        return;
    }

    void PrintFaults() {
        for (auto fault : faults) {
            cout << "Fault gate id: " << fault->gateId << endl;
            cout << "Fault value: " << fault->value - ZERO << endl;
        }
    }
};
FaultList* FaultList::instance = nullptr;

class AtpgEngine {
    unordered_map<GateId, AtpgValue> window;
    unordered_map<GateId, Value> testCube;
};

#endif  // HIATPG_MODEL_H
