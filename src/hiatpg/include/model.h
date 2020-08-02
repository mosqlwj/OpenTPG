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

//using namespace std;

using GateId = int;


enum Value
{
    X, ZERO, ONE, UNDEF
};

enum FaultType
{
    STUCK_AT_0, STUCK_AT_1
};

enum FaultStatus
{
    TESTED, UNKNOW
};

//  DEF_GATE(name, id)
#define GATE_TYPE_DEFINETION()  \
    DEF_GATE(PI,    0)    \
    DEF_GATE(PO,    1)    \
    DEF_GATE(AND,   2)    \
    DEF_GATE(NAND,  3)    \
    DEF_GATE(OR,    4)    \
    DEF_GATE(NOR,   5)    \
    DEF_GATE(XOR,   6)    \
    DEF_GATE(XNOR,  7)    \
    DEF_GATE(INV,   8)    \
    DEF_GATE(BUF,   9)

enum GateType : int8_t
{
    //PI, PO, AND, NAND, OR, NOR, XOR, XNOR, INV, BUF
    UNKNOWN = -1,
#define DEF_GATE(name, id)  name = id,
    GATE_TYPE_DEFINETION()
#undef DEF_GATE
};

static inline const char* GateNameOf(GateType type)
{
    switch (type)
    {
#define DEF_GATE(name, id)  case name: return #name;
        GATE_TYPE_DEFINETION()
#undef DEF_GATE
        default:
            return "UNK";
    }
}

static inline GateType GateTypeOf(const char* name)
{
    static std::unordered_map<std::string, GateType> mapper {
#define DEF_GATE(name, id) {#name, name},
        GATE_TYPE_DEFINETION()
#undef DEF_GATE
    };

    auto itr = mapper.find(std::string(name));
    if (itr == mapper.end())
    {
        return UNKNOWN;
    }

    return itr->second;
}



enum EventDir
{
    FORWARD,
    BACKWORD,
    BOTH,
};

struct Options
{
};



struct Gate
{
    GateId gateId;

    GateType type;

    std::vector<Gate*> fanins;

    std::vector<Gate*> fanouts;

    std::string name;

    Gate(GateId gateId, GateType type, const std::string& name) : gateId(gateId), type(type), name(name)
    {}
};



struct Fault
{
    GateId gateId;

    Value value;

    FaultType faultType;

    FaultStatus faultStatus;

    int index;

    bool detected;

    int faultSitePin;

    Fault(GateId gateId, int index, Value value) : gateId(gateId), index(index), value(value), detected(false)
    {}

    FaultStatus GetFaultStatusType() const
    { return faultStatus; }

    int GetFaultSitePin() const
    { return faultSitePin; }

    FaultType GetFaultType() const
    { return faultType; }

    int GetFaultSiteGateId() const
    { return gateId; }
};



struct AtpgValue
{
    Value goodVal;
    Value faultyVal;
};


struct Cube
{
    Cube* next;
    Cube* prev;
    int32_t faultIndex;
};

struct Pattern
{
    Pattern* next;
    Pattern* prev;
};


#endif  // HIATPG_MODEL_H
