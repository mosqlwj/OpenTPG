#include "SimFault.h"

SimFault::SimFault(SimGM* gm, NetlistParser* parser, vector<FaultDescript*>* fList)
	: netlistParser(parser)
	, faultList(fList)
	, goodMechine(gm)
{
	faultMachine = new uint64_t[parser->GetFlattenGates().size()];
	memset(faultMachine, 0, sizeof(uint64_t) * size);
}

SimFault::~SimFault()
{
	if (faultMachine != nullptr) {
		delete[] faultMachine;
		faultMachine = nullptr;
	}
}

SimFault::DoSim(uint64_t ma)
{
	mask = ma;
	for (auto fault : *faultList) {
		if (fault->GetFaultType() == ENUM::TESTED || fault->GetFaultType() == ENUM::AU_UNTESTABLE
			|| fault->GetFaultType() == ENUM::UNTESTABLE) {
			continue;
		}
		DoOneFaultSim(fault);
	}
}

SimFault::DoOneFaultSim(FaultDescript* fault)
{
	vector<Gate*>& flattenGate = netlistParser->GetFlattenGates();
	int32_t faultGateId = fault->GetFaultSiteGateId();
	uint64_t curVal = (*goodMechine)[faultGateId];
	faultMachine[faultGateId] = DoFaultGateSim(fault, flattenGate[faultGateId]);
	uint64_t newVal = faultMachine[faultGateId];

	if (curVal == newVal) {
		return;
	}
	else if (flattenGates[faultGateId]->GetGateType() == ENUM::PO) {
		fauDes->SetFaultStatusType(ENUM::TESTED);
		return;
	}
	queue<Gate*> que;
	que.enqueue(flattenGates[faultGateId]);

	while (!queue.empty()) {
		Gate* curGate = queue.dequeue();
		for (int i = 0; i < curGate->NumFanouts(); i++) {
			Gate* fanout = curGate->GetFanoutGate(i);
			uint64_t curVal = (*goodMechine)[fanout->GetGateId()];
			uint64_t newVal = SimGate(fanout);
			if (curVal != newVal) {
				if (fanout->GetGateType() == ENUM::PO) {
					fauDes->SetFaultStatusType(ENUM::TESTED);
					return;
				}
				que.enqueue(fanout);
			}
		}
	}
}

uint64_t SimFault::DoFaultGateSim(FaultDescript* fault, Gate* gate)
{
	if (fault->GetFaultSizePin() == 0) {
		faultMachine[gate->GetGateId()] = 
	}
}
