/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#ifndef OPENTPG_SIMUTIL_H
#define OPENTPG_SIMUTIL_H

#include "Gate.h"
#include "TestCube.h"
#include "common.h"
#include "printers.h"

#include <cstring>

class ValueManager {
    uint8_t* exists;
    size_t gCount;
    size_t cCount;

public:
    ValueManager() {
        exists = nullptr;
    }
    ~ValueManager() {
        Delete();
    }

    inline int Init(size_t gateCount) {
        if (exists == nullptr) {
            exists = (uint8_t*)malloc(sizeof(uint8_t) * (gateCount / 8 + 1));
        }
        gCount = gateCount;
        Reset();
        return 0;
    }

    inline void Delete() {
        free(exists);
    }

    inline void Reset() {
        memset(exists, 0, sizeof(uint8_t) * (gCount / 8 + 1));
    }

    inline bool Contains(int index) const {
        static uint8_t containsMask[8] = {
            0x01,
            0x02,
            0x04,
            0x08,
            0x10,
            0x20,
            0x40,
            0x80,
        };
        int byteIndex = index / 8;
        int bitsIndex = index % 8;
        return (0 != (exists[byteIndex] & containsMask[bitsIndex]));
    }

    inline void Set(int32_t index) {
        static uint8_t containsMask[8] = {
            0x01,
            0x02,
            0x04,
            0x08,
            0x10,
            0x20,
            0x40,
            0x80,
        };
        int byteIndex = index / 8;
        int bitsIndex = index % 8;
        exists[byteIndex] |= containsMask[bitsIndex];
    }
};

class SimUtil {
public:
    static Gate* GetStateDGate(Gate* state) {
        return (state->inputs)[1];
    }
    static Gate* GetStateClockGate(Gate* state) {
        return (state->inputs)[0];
    }
    static bool ClockPulse(TestCube* testCube, int32_t cycleId, Gate* state) {
        Gate* clockGate = SimUtil::GetStateClockGate(state);
        if (clockGate == nullptr) {
            return false;
        }
        const std::vector<std::vector<LogicVal>>& logicValue = testCube->GetLogicValue();
        if (logicValue[cycleId][clockGate->id] == LOGIC_1) {
            return true;
        }
        return false;
    }
    static LogicVal GetLogicVal(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, int32_t gateId) {
        if (goodMechine == nullptr) {
            return LOGIC_UNKNOW;
        }

        if (faultMechine == nullptr || (*faultMechine)[cycleId][gateId] == LOGIC_UNKNOW) {
            return (*goodMechine)[cycleId][gateId];
        }

        return (*faultMechine)[cycleId][gateId];
    }
    static void SetLogicVal(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, int32_t gateId, LogicVal res) {
        if (faultMechine == nullptr) {
            (*goodMechine)[cycleId][gateId] = res;
        } else {
            (*faultMechine)[cycleId][gateId] = res;
        }
    }
    static LogicVal ReverseValue(LogicVal val) {
        LogicVal res = LOGIC_UNKNOW;
        switch (val) {
            case LOGIC_X: {
                res = LOGIC_X;
                break;
            }
            case LOGIC_1: {
                res = LOGIC_0;
                break;
            }
            case LOGIC_0: {
                res = LOGIC_1;
                break;
            }
            case LOGIC_UNKNOW: {
                res = LOGIC_UNKNOW;
                break;
            }
            default: {
                break;
            }
        }
        return res;
    }
    static LogicVal CalculateGate(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate, const LogicVal logicTable[3][3]) {
        LogicVal res = GetLogicVal(goodMechine, faultMechine, cycleId, (gate->inputs)[0]->id);
        for (int i = 1; i < gate->inputs.size(); i++) {
            Gate* fanin = (gate->inputs)[i];
            res = logicTable[res][GetLogicVal(goodMechine, faultMechine, cycleId, fanin->id)];
        }
        return res;
    }
    static void SimINV(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        Gate* fanin = (gate->inputs)[0];
        LogicVal res = INV_TABLE[GetLogicVal(goodMechine, faultMechine, cycleId, fanin->id)];
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, res);
    }
    static void SimBUF(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        Gate* fanin = (gate->inputs)[0];
        LogicVal res = BUF_TABLE[GetLogicVal(goodMechine, faultMechine, cycleId, fanin->id)];
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, res);
    }
    static void SimAND(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, AND_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, res);
    }
    static void SimNAND(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, AND_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, ReverseValue(res));
    }
    static void SimOR(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, OR_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, res);
    }
    static void SimNOR(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, OR_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, ReverseValue(res));
    }
    static void SimXOR(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, XOR_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, res);
    }
    static void SimXNOR(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal res = CalculateGate(goodMechine, faultMechine, cycleId, gate, XOR_TABLE);
        SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, ReverseValue(res));
    }
    static void SimMUX(std::vector<std::vector<LogicVal>>* goodMechine, std::vector<std::vector<LogicVal>>* faultMechine, int32_t cycleId, Gate* gate) {
        LogicVal selValue = GetLogicVal(goodMechine, faultMechine, cycleId, ((gate->inputs)[0])->id);
        LogicVal dValue = GetLogicVal(goodMechine, faultMechine, cycleId, ((gate->inputs)[1])->id);
        LogicVal siValue = GetLogicVal(goodMechine, faultMechine, cycleId, ((gate->inputs)[2])->id);
        if (selValue == LOGIC_0) {
            SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, dValue);
        } else if (selValue == LOGIC_1) {
            SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, siValue);
        } else if (selValue == LOGIC_X) {
            if (dValue == siValue) {
                SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, siValue);
            } else {
                SetLogicVal(goodMechine, faultMechine, cycleId, gate->id, LOGIC_X);
            }
        }
    }
};

#endif //OPENTPG_SIMUTIL_H
