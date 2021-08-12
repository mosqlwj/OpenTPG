//
// Created by fish on 2021/6/28.
//

#ifndef HIPARSER_NETLIST_H
#define HIPARSER_NETLIST_H

#include "Fault.h"
#include "Gate.h"
#include "ScanChain.h"
#include "common.h"
#include <map>
#include <unordered_map>
#include <vector>

class Netlist {
public:
    int Parse(const std::string& name);
    const std::unordered_map<std::string, Gate*>& GetGateNameMap()
    {
        return name2Gate;
    }
    uint32_t GetPICount() const
    {
        return numOfPI;
    }

    uint32_t GetScanCellCount() const
    {
        return ((cubeEndId + 1) - numOfPI);
    }

    uint32_t GetCubeEndId() const
    {
        return cubeEndId;
    }

    const std::vector<Fault*>& GetFaultList() const
    {
        return faultlist;
    }

private:
    int32_t numOfPI = 0;
    int32_t numOfPO = 0;
    int32_t numOfDFF = 0;
    int32_t numOfGates = 0;
    uint32_t dffBegin = 0;
    uint32_t dffEnd = 0;
    uint32_t cubeEndId = 0;
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
    void ReOrderPi();
    void ReOrderDff();
    void DumpGates();
    void CheckFloating();
    bool ParseConfig();
    void CalcCubeRange();
};

#endif // HIPARSER_NETLIST_H
