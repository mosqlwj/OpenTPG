#pragma once
#include "SimInterface.h"

class SimFault : public SimInterface {
public:
    SimFault(SimGM* gm, NetlistParser* parser, vector<FaultDescript*>* fList);
    ~SimFault();

    virtual void DoSim(uint64_t ma);

private:
    void DoOneFaultSim(FaultDescript* fault);
    uint64_t DoFaultGateSim(FaultDescript* fault, Gate* gate);
    uint64_t SimFaultGate(Gate* gate);

private:
    NetlistParser* netlistParser;
    vector<FaultDescript*>* faultList;
    SimGM* goodMechine;
    ValueManager* faultValueManager;
    uint64_t* faultMachine;
    uint64_t mask;
};
