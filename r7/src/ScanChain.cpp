/**
 * Copyright (c) [Year] [name of copyright holder]
 * [Software Name] is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "ScanChain.h"
#include <iostream>

ScanChain::ScanChain(std::string chainName, Gate* si, Gate* so)
    : chainName(chainName)
    , si(si)
    , so(so)
{
}

void ScanChain::Trace(const std::unordered_map<std::string, Gate*>& gateNameMap)
{
    Gate* soGate = gateNameMap.at(so->name);
    Gate* curGate = soGate->inputs[0];
    while (curGate->name != si->name) {
        switch (curGate->type) {
        case DFF:
            chainGates.push_front(curGate);
            curGate = curGate->inputs[1];
            break;
        case MUX:
            curGate = curGate->inputs[2];
            break;
        case BUF:
        case INV:
            curGate = curGate->inputs[0];
        default:
            std::cerr << "Scan chain should not contains gate beyond dff and mux, current gate type is: " << curGate->type << std::endl;
            exit(-1);
            break;
        }
    }
}
