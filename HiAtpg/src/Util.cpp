//
// Created by fish on 2021/6/30.
//

#include "Util.h"
#include "asserts.h"

#include <iostream>
#include <map>

std::map<std::string, GateType> name2Type = {
#define DEF_GATETYPE(id, name, str) { str, GateType::name },
    GATETYPE_TABLE()
#undef DEF_GATETYPE
};

GateType Util::GetGateTypeFromString(const std::string& name)
{
    if (name2Type.find(name) == name2Type.end()) {
        std::cerr << "Not support gate type: " << name << std::endl;
        ASSERT(false);
        return UNKNOWN;
    }

    return name2Type[name];
}
