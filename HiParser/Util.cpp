//
// Created by fish on 2021/6/30.
//

#include "Util.h"
#include <map>
#include <iostream>

std::map<std::string, GateType> name2Type = {{"INPUT", PI}, {"OUTPUT", PO}, {"AND", AND}, {"NAND", NAND},
                                             {"OR", OR}, {"NOR", NOR}, {"XOR", XOR}, {"XNOR", XNOR},
                                             {"NOT", INV}, {"BUF", BUF}, {"MUX", MUX}, {"DFF", DFF}};

GateType Util::GetGateTypeFromString(const std::string &name) {
    if (name2Type.find(name) == name2Type.end()) {
        std::cerr << "Not support gate type: " << name << std::endl;
        return UNKNOWN;
    }
    return name2Type[name];
}
