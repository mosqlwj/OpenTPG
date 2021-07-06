//
// Created by fish on 2021/7/31.
//

#ifndef HIATPG_SCANCHAIN_H
#define HIATPG_SCANCHAIN_H

#include "Gate.h"
#include <unordered_map>
#include <list>

class ScanChain {
public:
    ScanChain(std::string chainName, Gate* si, Gate* so);
    void Trace(const std::unordered_map<std::string, Gate*>& gateNameMap);
    std::list<Gate*>& GetChainGates() { return chainGates; }

private:
    Gate* si;
    Gate* so;
    std::string chainName;
    std::list<Gate*> chainGates;
};


#endif //HIATPG_SCANCHAIN_H
