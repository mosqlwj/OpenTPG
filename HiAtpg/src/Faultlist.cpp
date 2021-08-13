//
// Created by luolijun on 2021/8/12.
//
#include "Faultlist.h"

#include "Gate.h"
#include "Netlist.h"

#include "asserts.h"
#include "common.h"
#include "printers.h"

int Faultlist::CreateFaults(Netlist* netlist)
{
    ASSERT(netlist != nullptr);
    ASSERT(netlist->Gates().size() > 0);

    FaultType faultType;
    for (const Gate* gate : netlist->Gates()) {
        ASSERT(gate != nullptr);

        if (gate->inputs.size() > 1) {
            // add s-a-1 for AND/NAND, add s-a-0 for OR/NOR, add both for other gate.
            faultType = (gate->type & (AND | NAND)) ? STUCK_AT_1 : STUCK_AT_0;
            for (int inputIdx = 0; inputIdx < gate->inputs.size(); inputIdx++) {
                if (gate->inputs[inputIdx]->outputs.size() > 1) {
                    faultlist.push_back(new Fault(faultType, gate, inputIdx + 1));
                    if (!(gate->type & (AND | NAND | OR | NOR))) {
                        faultlist.push_back(new Fault(faultType == STUCK_AT_0 ? STUCK_AT_1 : STUCK_AT_0, gate, inputIdx + 1));
                    }
                }
            }
        }

        if ((gate->outputs.size() == 1) && ((gate->outputs[0]->inputs.size() > 1) || (gate->outputs[0]->type == PO))) {
            faultType = (gate->outputs[0]->type & (OR | NOR)) ? STUCK_AT_0 : STUCK_AT_1;
            faultlist.push_back(new Fault(faultType, gate, 0));
            if (!(gate->type & (AND | NAND | OR | NOR))) {
                faultlist.push_back(new Fault(faultType == STUCK_AT_0 ? STUCK_AT_1 : STUCK_AT_0, gate, 0));
            }
        } else if (gate->outputs.size() > 1) {
            faultlist.push_back(new Fault(STUCK_AT_1, gate, 0));
            faultlist.push_back(new Fault(STUCK_AT_0, gate, 0));
        } else if (gate->type == PO && gate->inputs[0]->outputs.size() > 1) {
            faultlist.push_back(new Fault(STUCK_AT_1, gate, 1));
            faultlist.push_back(new Fault(STUCK_AT_0, gate, 1));
        }
    }

    //  所有 fault 编号
    for (int32_t i = 0; i < faultlist.size(); i++) {
        faultlist[i]->id = i;
    }

    return 0;
}

int Faultlist::DumpFaults(const std::string& faultlistFile)
{
    if (faultlistFile.empty()) {
        ASSERT(false);
        return -1;
    }

    std::ofstream stream(faultlistFile);
    if (!stream.is_open()) {
        ASSERT(false);
        return -1;
    }

    for (std::size_t i = 0; i < faultlist.size(); i++) {
        ASSERT(faultlist[i] != nullptr);
        stream << *(faultlist[i]) << std::endl;
    }

    return 0;
}

void Faultlist::Clean()
{
    for (Fault*& fault : faultlist) {
        delete fault;
        fault = nullptr;
    }

    faultlist.resize(0);
}
