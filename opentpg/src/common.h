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
#ifndef OPENTPG_COMMON_H
#define OPENTPG_COMMON_H

#include "asserts.h"

#include <iostream>
#include <stdint.h>

typedef int32_t GateId;
typedef int32_t FaultId;

// clang-format off
#define GATETYPE_TABLE()                    \
    DEF_GATETYPE(0,     PI,     "INPUT")    \
    DEF_GATETYPE(1,     DFF,    "DFF")      \
    DEF_GATETYPE(2,     MUX,    "MUX")      \
    DEF_GATETYPE(3,     AND,    "AND")      \
    DEF_GATETYPE(4,     NAND,   "NAND")     \
    DEF_GATETYPE(5,     OR,     "OR")       \
    DEF_GATETYPE(6,     NOR,    "NOR")      \
    DEF_GATETYPE(7,     XOR,    "XOR")      \
    DEF_GATETYPE(8,     XNOR,   "XNOR")     \
    DEF_GATETYPE(9,     INV,    "NOT")      \
    DEF_GATETYPE(10,    BUF,    "BUF")      \
    DEF_GATETYPE(11,    PO,     "OUTPUT")   \
    /* (end) */
// clang-format on

// clang-format off
#define FAULTSTATUS_TABLE()                                                                         \
    DEF_FAULTSTATUS(0, INIT,                    "UC.UNK",   "initial status")                       \
    DEF_FAULTSTATUS(1, TESTED,                  "DS",       "fault can be detected")                \
    DEF_FAULTSTATUS(2, UNTESTABLE,              "AU",       "untestable")                           \
    DEF_FAULTSTATUS(3, REDUNDANT,               "RE",       "redundant fault")                      \
    DEF_FAULTSTATUS(4, DETECT_BY_IMPLICATION,   "DI",       "fault can be detected by implication") \
    /* (end) */
// clang-format on

// clang-format off
//  DEF_LOGICVAL(id, name, str)
#define LOGICVAL_TABLE()            \
    DEF_LOGICVAL(0, LOGIC_X, 'x')     \
    DEF_LOGICVAL(1, LOGIC_0, '1')     \
    DEF_LOGICVAL(2, LOGIC_1, '0')     \
    /* (end) */
// clang-format on

enum GateType : uint32_t {

#define DEF_GATETYPE(id, name, str) name = 1 << id,
    GATETYPE_TABLE()
#undef DEF_GATETYPE

    //
    UNKNOWN
    = 0x1000,
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

enum LogicVal : uint8_t {
#define DEF_LOGICVAL(id, name, str) name = id,
    LOGICVAL_TABLE()
#undef DEF_LOGICVAL

    //
    LOGIC_COUNT
};

enum FaultStatus : int8_t {
#define DEF_FAULTSTATUS(id, name, str, desc) name = id,
    FAULTSTATUS_TABLE()
#undef DEF_FAULTSTATUS
};

static char charOfLogicVal(LogicVal val)
{
    switch (val) {
#define DEF_LOGICVAL(id, name, chr) \
    case LogicVal::name:            \
        return chr;
        LOGICVAL_TABLE()
#undef DEF_LOGICVAL

    default:
        ASSERT(false);
        perror("don't support logic_undefine, during logic_val convert to char");
        return 'x';
    }
}

static const char* charOfFaultStatus(FaultStatus status)
{
    switch (status) {

#define DEF_FAULTSTATUS(id, name, str, desc) \
    case FaultStatus::name:                  \
        return str;
        FAULTSTATUS_TABLE()
#undef DEF_FAULTSTATUS
    default:
        ASSERT(false);
        std::cerr << "Unknown fault type" << std::endl;
        return "";
    }
}

#endif //OPENTPG_COMMON_H
