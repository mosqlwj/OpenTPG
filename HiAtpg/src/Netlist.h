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
    int LoadNetlist(const std::string& netlistFileName, const std::string& cfgFileName);
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
    std::vector<Gate*> gates;    // 包含bench中所有的gate
    std::vector<Fault*> faultlist;  // ATPG需要target的所有的fault，已经做了故障折叠处理
    std::unordered_map<std::string, Gate*> name2Gate;  // 存储gate name到Gate*的映射关系
    std::vector<ScanChain*> scanChains;  // 存储cfg中定义的scan chain，已经trace出chain上包含的所有dff
    std::map<Gate*, LogicVal> clk2OffVal;  // 存储cfg中定义的时钟以及时钟对应的off value

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
