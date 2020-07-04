//
// Created by fish on 2020/7/4.
//

#ifndef HIATPG_COMMON_H
#define HIATPG_COMMON_H

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



#endif //HIATPG_COMMON_H
