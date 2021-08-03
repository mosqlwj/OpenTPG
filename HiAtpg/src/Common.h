//
// Created by fish on 2021/6/30.
//

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef int32_t GateId;

enum GateType : uint32_t {
    PI = 0x1,
    DFF = 0x2,
    MUX = 0x4,
    AND = 0x8,
    NAND = 0x10,
    OR = 0x20,
    NOR = 0x40,
    XOR = 0x80,
    XNOR = 0x100,
    INV = 0x200,
    BUF = 0x400,
    PO = 0x800,
    UNKNOWN = 0x1000,
};

enum FaultType {
    STUCK_AT_0,
    STUCK_AT_1
};

enum LogicVal {
    LOGIC_X,
    LOGIC_0,
    LOGIC_1,
    LOGIC_UNDEF
};

#endif
