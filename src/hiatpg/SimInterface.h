#pragma once

class SimEntryInterface {
public:
    virtual void HandleTestCube(unordered_map<GateId, Value> testCube) = 0;
    virtual void HandleOutput();
    virtual void ParsePattern();
};

class SimInterface {
public:
    virtual void DoSim() = 0;
};