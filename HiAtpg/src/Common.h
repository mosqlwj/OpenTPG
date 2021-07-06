//
// Created by fish on 2021/6/30.
//

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

typedef int32_t GateId;

enum GateType {
    PI,
    DFF,
    PO,
    AND,
    NAND,
    OR,
    NOR,
    XOR,
    XNOR,
    INV,
    BUF,
    MUX,
    UNKNOWN,
};

enum FaultType {
    STUCK_AT_0,
    STUCK_AT_1
};

#endif
