#pragma once

#include "SimFault.h"
#include "SimGood.h"
#include "SimInterface.h"
#include "SimUtil.h"

class SimEntry : public SimEntryInterface {
public:
    SimEntry();
    ~SimEntry();
    virtual void HandleTestCube(unordered_map<GateId, Value> testCube);

private:
    NetlistParser* netlistParser;
    SimGood* goodSimulator;
    SimFault* faultSimulator;
    vector<FaultDescripotr*> faultList;
    uint64_t mask;
};
