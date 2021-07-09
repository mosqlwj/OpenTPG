//
// Created by fish on 2021/6/28.
//

#ifndef HIPARSER_NETLIST_H
#define HIPARSER_NETLIST_H

#include <unordered_map>
#include "Common.h"
#include <vector>
#include "Gate.h"
#include "Fault.h"

class Netlist {
public:
    void Parse(const std::string& name);
    void CreateFaultlist();
    void CreateFaultByGate(const Gate *gate, const std::vector<std::string> &pinName);
    void CollapseFault();
    std::string GetFaultName(Gate* gate, int32_t pinIdx);
private:
    std::vector<Gate*> gates;
    std::vector<Fault*> faultlist;
    std::unordered_map<Fault*, std::vector<int32_t>> collapsedFaultlist;
    int32_t numOfPI;
    int32_t numOfPO;
    int32_t numOfGates;
    std::unordered_map<std::string, Gate*> name2Gate;
};


#endif //HIPARSER_NETLIST_H
