//
// Created by fish on 2021/6/30.
//

#ifndef HIPARSER_GATE_H
#define HIPARSER_GATE_H

#include "common.h"
#include <string>
#include <vector>

struct Gate {
public:
    Gate(const std::string& name, GateType type);

    GateId id;
    std::string name;
    GateType type;
    std::vector<Gate*> inputs;
    std::vector<Gate*> outputs;
    uint32_t level;
};

#endif //HIPARSER_GATE_H
