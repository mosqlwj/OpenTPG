//
// Created by fish on 2021/6/30.
//

#ifndef HIPARSER_UTIL_H
#define HIPARSER_UTIL_H

#include "Common.h"
#include <string>

class Util {
public:
    static GateType GetGateTypeFromString(const std::string& name);
};


#endif //HIPARSER_UTIL_H
