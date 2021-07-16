//
// Created by fish on 2021/6/30.
//

#include "Util.h"
#include <map>
#include <iostream>
#include <regex>

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

std::vector<std::string> Util::split(const std::string &input, const std::string &delim) {
    std::regex re {delim};
    return std::vector<std::string> {
            std::sregex_token_iterator(input.begin(), input.end(), re, -1),
            std::sregex_token_iterator()
    };
}

void Util::trim(std::string& s) {
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_first_not_of(" ") + 1);
}
