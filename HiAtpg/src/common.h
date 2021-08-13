//
// Created by fish on 2021/6/30.
//

#ifndef COMMON_H
#define COMMON_H

#include "asserts.h"

#include <iostream>
#include <stdint.h>

typedef int32_t GateId;
typedef int32_t FaultId;

#define GATETYPE_TABLE()           \
    DEF_GATETYPE(0, PI, "INPUT")   \
    DEF_GATETYPE(1, DFF, "DFF")    \
    DEF_GATETYPE(2, MUX, "MUX")    \
    DEF_GATETYPE(3, AND, "AND")    \
    DEF_GATETYPE(4, NAND, "NAND")  \
    DEF_GATETYPE(5, OR, "OR")      \
    DEF_GATETYPE(6, NOR, "NOR")    \
    DEF_GATETYPE(7, XOR, "XOR")    \
    DEF_GATETYPE(8, XNOR, "XNOR")  \
    DEF_GATETYPE(9, INV, "NOT")    \
    DEF_GATETYPE(10, BUF, "BUF")   \
    DEF_GATETYPE(11, PO, "OUTPUT") \
    /* end */

enum GateType : uint32_t {

#define DEF_GATETYPE(id, name, str) name = 1 << id,
    GATETYPE_TABLE()
#undef DEF_GATETYPE

        UNKNOWN
    = 0x1000, //
};

static inline const char* StringOf(GateType type)
{
    switch (type) {
#define DEF_GATETYPE(id, name, str) \
    case GateType::name:            \
        return str;
        GATETYPE_TABLE()
#undef DEF_GATETYPE
    default:
        ASSERT(false);
        return "??";
    }
}

enum FaultType {
    STUCK_AT_0,
    STUCK_AT_1
};

enum LogicVal {
    LOGIC_UNDEF = -1,
    LOGIC_X = 0,
    LOGIC_0 = 1,
    LOGIC_1 = 2,

    //
    LOGIC_COUNT
};

enum FaultStatus {
    INIT,                 // initial status
    TESTED,               // fault can be detected
    UNTESTABLE,           // untestable
    REDUNDANT,            // redundant fault
    DETECT_BY_IMPLICATION // fault can be detected by implication
};

static char charOfLogicVal(LogicVal val)
{
    switch (val) {
    case LOGIC_X:
        return 'x';
    case LOGIC_1:
        return '1';
    case LOGIC_0:
        return '0';
    default:
        perror("don't support logic_undefine, during logic_val convert to char");
        return 'x';
    }
}

static const char* charOfFaultStatus(FaultStatus status)
{
    switch (status) {
    case INIT:
        return "UC.UNK";
    case TESTED:
        return "DS";
    case UNTESTABLE:
        return "AU";
    case REDUNDANT:
        return "RE";
    case DETECT_BY_IMPLICATION:
        return "DI";
    default:
        std::cerr << "Unknown fault type" << std::endl;
        break;
    }

    return "";
}

#endif
