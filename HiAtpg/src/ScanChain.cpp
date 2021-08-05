//
// Created by fish on 2021/7/31.
//

#include "ScanChain.h"
#include <iostream>

ScanChain::ScanChain(std::string chainName, Gate *si, Gate *so) : chainName(chainName), si(si), so(so) {

}

void ScanChain::Trace(const std::unordered_map<std::string, Gate*>& gateNameMap) {
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
