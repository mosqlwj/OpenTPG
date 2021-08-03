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
#include <map>
#include "ScanChain.h"

class Netlist {
public:
    void Parse(const std::string& name);
    const std::unordered_map<std::string, Gate*>& GetGateNameMap() { return name2Gate; }

private:
    int32_t numOfPI = 0;
    int32_t numOfPO = 0;
    int32_t numOfDFF = 0;
    int32_t numOfGates = 0;
    uint32_t dffBegin = 0;
    uint32_t dffEnd = 0;
    std::vector<Gate*> gates;
    std::vector<Fault*> faultlist;
    std::unordered_map<std::string, Gate*> name2Gate;
    std::vector<ScanChain*> scanChains;
    std::map<Gate*, LogicVal> clk2OffVal;

private:
    void CreateFaultlist();
    void SaveFaultlist();
    void SortGates();
    void TagGateRange();
    void ReOrderDff();
    void DumpGates();
    void CheckFloating();
    bool ParseConfig();
};


#endif //HIPARSER_NETLIST_H
