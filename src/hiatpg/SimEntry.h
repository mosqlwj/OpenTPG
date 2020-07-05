#pragma once

#include "SimFault.h"
#include "SimGood.h"
#include "SimUtil.h"
#include "SimInterface.h"

class SimEntry : public SimEntryInterface
{
public:
	SimEntry();
	~SimEntry();
	virtual void HandleTestCube(vector<Cube> testCube);

private:
	NetlistParser* netlistParser;
	SimGood* goodSimulator;
	SimFault* faultSimulator;
	vector<FaultDescripotr*> faultList;
	uint64_t mask;
};

