//
// Created by fish on 2021/7/6.
//

#ifndef HIATPG_FAULT_H
#define HIATPG_FAULT_H

#include <cstdint>
#include <utility>
#include "Common.h"
#include "Gate.h"

struct Fault {
    int32_t id;
    FaultType type;
    const Gate* gate;
    int32_t pin;   // output: 0, input: 1~n
    std::string pinName;

    Fault(FaultType type, const Gate* gate, int32_t pin, std::string pinName) : id(-1), type(type), gate(gate), pin(pin), pinName(std::move(pinName)) {}
};


#endif //HIATPG_FAULT_H
