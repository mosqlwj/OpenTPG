#pragma once
#include "SimInterface.h"

class SimGM
{
	uint64_t* res;
public:
	SimGM(uint64_t size) 
	{
		res = new uint64_t[parser->GetFlattenGates().size()];
		memset(res, 0, sizeof(uint64_t) * size);
	}
	~SimGM()
	{
		if (nullptr != res) {
			delete[] res;
			res = nullptr;
		}
	}
	uint64_t& operator[](int index)
	{
		return res[index];
	}
};

class SimGood
{
public:
	SimGood(NetlistParser* parser);
	~SimGood();

	virtual void DoSim(uint64_t mask);
	inline SimGM* GetGoodMechine() const { return goodMechine; }

private:
	NetlistParser* netlistParser;
	ValueManager* valueManager;
	SimGM* goodMechine;
};

