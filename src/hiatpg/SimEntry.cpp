#include "SimEntry.h"
#include "SimGood.h"
#include "SimFault.h"

SimEntry::SimEntry():mask(0)
{
	goodSimulator = new SimGood(netlistParser);
	faultSimulator = new SimFault(goodSimulator->GetGoodMechine(), netlistParser, faultList);
}

SimEntry::~SimEntry()
{
	if (nullptr != goodSimulator) {
		delete goodSimulator;
		goodSimulator = nullptr;
	}

	if (nullptr != faultSimulator) {
		delete faultSimulator;
		faultSimulator = nullptr;
	}
}

SimEntry::HandleTestCube(vector<Cube> testCube)
{
	if (testCube.size() > 64) {
		cout << "test cube size error" << endl;
		return;
	}

	mask = UINT64_MAX >> (PARALLCUBESIZEMAX - testCube.size());

	goodSimulator->DoSim(mask);
	faultSimulator->DoSim(mask);
}