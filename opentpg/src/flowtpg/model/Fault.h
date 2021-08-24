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
#ifndef OPENTPG_FAULT_H
#define OPENTPG_FAULT_H

#include "Gate.h"
#include "common.h"
#include <cstdint>
#include <utility>

struct Fault {
    int32_t id;
    FaultType type;
    const Gate* gate;
    int32_t pin; // output: 0, input: 1~n
    std::string pinName;

    Fault(FaultType type, const Gate* gate, int32_t pin)
        : id(-1)
        , type(type)
        , gate(gate)
        , pin(pin)
        , pinName(GetFaultName(gate, pin))
    {
    }
    std::string GetFaultName(const Gate* gate, int32_t pinIdx);
};

#endif //OPENTPG_FAULT_H
