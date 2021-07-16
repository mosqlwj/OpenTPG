//
// Created by fish on 2021/6/28.
//

#ifndef HIPARSER_NETLIST_H
#define HIPARSER_NETLIST_H

#include "Common.h"
#include <vector>
#include "Gate.h"
#include "Fault.h"
#include <unordered_map>

class Netlist {
public:
    void Parse(const std::string& name);

private:
    std::vector<Gate*> gates;
    std::vector<Fault*> faultlist;
    int32_t numOfPI;
    int32_t numOfPO;
    int32_t numOfGates;
    std::unordered_map<std::string, Gate*> name2Gate;

private:
    void CreateFaultlist();
    void SaveFaultlist();
    void SortGates();
    void CheckFloating();
};


#endif //HIPARSER_NETLIST_H
