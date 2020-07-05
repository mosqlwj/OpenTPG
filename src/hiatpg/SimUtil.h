#pragma once

const int PARALLCUBESIZEMAX = 64;

class ValueManager
{
	uint64_t* values;
	uint64_t* exists;
	size_t count;
	uint64_t gBase;

public:

	ValueManager()
	{
		values = nullptr;
		exists = nullptr;
		count = 0;
		gBase = 0;
	}

	~ValueManager()
	{
		Delete();
	}

	inline int Init(size_t maxGateCount)
	{
		values = new uint64_t[maxGateCount];
		exists = new uint64_t[maxGateCount];
		count = maxGatecount;
		Reset();
		return 0;
	}

	inline void Delete()
	{
		delete[] values;
		delete[] exists;
	}

	inline void Reset()
	{
		gBase = 0;
		memset(exists, 0, sizeof(uint64_t) * count);
	}

	inline bool Contains(int index) const 
	{
		return exists[index] > gBase;
	}

	inline uint64_t IndexOf(int32_t index) const
	{
		return values[index];
	}

	inline void Set(int32_t index, uint64_t val)
	{
		values[index] = val;
		exists[index] = gBase + 1£»
	}

	inline void Remove(int32_t index)
	{
		exists[index] = 0;
	}

	inline void SetGlobalBase()
	{
		gBase++;
	}
};

class SimUtil
{
public:
	SimGate(Gate* gate, uint64_t* goodMechine, uint64_t mask);
	SimGate1(Gate* gate, uint64_t* goodMechine, uint64_t mask);
	SimGate2(Gate* gate, uint64_t* goodMechine, uint64_t mask);
	SimGate3(Gate* gate, uint64_t* goodMechine, uint64_t mask);
	SimGate4(Gate* gate, uint64_t* goodMechine, uint64_t mask);
};

