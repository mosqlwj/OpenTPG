#pragma once

const int PARALLCUBESIZEMAX = 64;
enum GateType { PI = 0, PO, AND, NAND, OR, NOR, XOR, NXOR, INV, MUX, BUF };

class ValueManager {
    uint64_t* values;
    uint64_t* exists;
    size_t count;
    uint64_t gBase;

public:
    ValueManager() {
        values = nullptr;
        exists = nullptr;
        count = 0;
        gBase = 0;
    }

    ~ValueManager() { Delete(); }

    inline int Init(size_t maxGateCount) {
        values = new uint64_t[maxGateCount];
        exists = new uint64_t[maxGateCount];
        count = maxGatecount;
        Reset();
        return 0;
    }

    inline void Delete() {
        delete[] values;
        delete[] exists;
    }

    inline void Reset() {
        gBase = 0;
        memset(exists, 0, sizeof(uint64_t) * count);
    }

    inline bool Contains(int index) const { return exists[index] > gBase; }

    inline uint64_t IndexOf(int32_t index) const { return values[index]; }

    inline void Set(int32_t index, uint64_t val) {
        values[index] = val;
        exists[index] = gBase + 1£»
    }

    inline void Remove(int32_t index) { exists[index] = 0; }

    inline void SetGlobalBase() { gBase++; }
};

class SimGM {
    uint64_t* res;

public:
    SimGM(uint64_t size) {
        res = new uint64_t[parser->GetFlattenGates().size()];
        memset(res, 0, sizeof(uint64_t) * size);
    }
    ~SimGM() {
        if (nullptr != res) {
            delete[] res;
            res = nullptr;
        }
    }
    uint64_t& operator[](int index) { return res[index]; }
};

class SimUtil {
public:
    // simulation gate's value acrroding goodmechine
    // and set goodvalue to mechine same time
    uint64_t SimGate(Gate* gate, SimGM* goodMechine, uint64_t mask);
    uint64_t SimGate1(Gate* gate, SimGM* goodMechine, uint64_t mask);
    uint64_t SimGate2(Gate* gate, SimGM* goodMechine, uint64_t mask);
    uint64_t SimGate3(Gate* gate, SimGM* goodMechine, uint64_t mask);
    uint64_t SimGate4(Gate* gate, SimGM* goodMechine, uint64_t mask);

    // siulation gate's fault vale acrroding goodmechine/faultmechine/valuemanager
    // and set fault machine at same time
    // notice:not change good machine ever
    uint64_t SimFaultGate1(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine, ValueManager* faultValueManager,
                           uint64_t mask);
    uint64_t SimFaultGate2(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine, ValueManager* faultValueManager,
                           uint64_t mask);
    uint64_t SimFaultGate3(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine, ValueManager* faultValueManager,
                           uint64_t mask);
    uint64_t SimFaultGate4(Gate* gate, SimGM* goodMechine, uint64_t* faultMechine, ValueManager* faultValueManager,
                           uint64_t mask);
};
