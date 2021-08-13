//
// Created by fish on 2021/6/30.
//

#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <stdint.h>

typedef int32_t GateId;
typedef int32_t FaultId;

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
    LOGIC_UNDEF = -1,
    LOGIC_X = 0,
    LOGIC_0 = 1,
    LOGIC_1 = 2,

    //
    LOGIC_COUNT
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

#endif
