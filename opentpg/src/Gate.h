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
#ifndef OPENTPG_GATE_H
#define OPENTPG_GATE_H

#include "common.h"
#include <string>
#include <vector>

struct Gate {
    Gate(const std::string& name, GateType type);

    GateId id;
    std::string name;
    GateType type;
    std::vector<Gate*> inputs;
    std::vector<Gate*> outputs;
    uint32_t level;
};

#endif //R7_GATE_H
