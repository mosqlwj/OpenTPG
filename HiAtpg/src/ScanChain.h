//
// Created by fish on 2021/7/31.
//

#ifndef HIATPG_SCANCHAIN_H
#define HIATPG_SCANCHAIN_H

#include "Gate.h"

class ScanChain {
public:
    ScanChain(std::string chainName, Gate* si, Gate* so);

private:
    Gate* si;
    Gate* so;
    std::string chainName;
};


#endif //HIATPG_SCANCHAIN_H
