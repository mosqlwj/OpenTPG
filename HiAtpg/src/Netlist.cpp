//
// Created by fish on 2021/6/28.
//

#include "Netlist.h"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Util.h"
#include "Params.h"

void Netlist::Parse(const std::string &fileName) {
    std::string line;
    const std::string commentHead = "#";
    const std::string inputHead = "INPUT";
    const std::string outputHead = "OUTPUT";

    std::ifstream netlist(fileName);
    // 1st read by line, parse all gate
    while (getline(netlist, line)) {
        if (line.empty()) {
            continue;
        }

        if (boost::starts_with(line, commentHead)) {
            continue;
        }

        std::vector<std::string> res;
        boost::split(res, line, boost::is_any_of("(),= "), boost::token_compress_on);

        // eg: INPUT(U1)
        if (inputHead == res[0]) {
            Gate* gate = new Gate(res[1], Util::GetGateTypeFromString(res[0]));
            gates.push_back(gate);
            name2Gate[res[1]] = gate;
            numOfPI++;
            continue;
        }

        // eg: OUTPUT(U1)
        if (outputHead == res[0]) {
            Gate* gate = new Gate(res[1] + "_PO", Util::GetGateTypeFromString(res[0]));
            gates.push_back(gate);
            name2Gate[res[1]] = gate;
            numOfPO++;
            continue;
        }

        // eg: U1 = AND(U2, U3);
        Gate* gate = new Gate(res[0], Util::GetGateTypeFromString(res[1]));
        gates.push_back(gate);
        name2Gate[res[0]] = gate;
        numOfGates++;
    }

    netlist.clear();
    netlist.seekg(0, std::ios::beg);

    // 2nd read by line, parse connection
    while (getline(netlist, line)) {
        if (line.empty()) {
            continue;
        }

        if (boost::starts_with(line, commentHead)) {
            continue;
        }

        std::vector<std::string> res;
        boost::split(res, line, boost::is_any_of("(),= "), boost::token_compress_on);

        if (inputHead ==res[0]) {
            continue;
        }

        if (outputHead ==res[0]) {
            continue;
        }

        Gate* curGate = name2Gate[res[0]];
        int32_t inputStartIdx = 2;
        for (int32_t i = inputStartIdx; i < res.size(); i++) {
            if (name2Gate.find(res[i]) == name2Gate.end()) {
                continue;
            }
            Gate* gate = name2Gate[res[i]];
            curGate->inputs.push_back(gate);
            gate->outputs.push_back(curGate);
        }
    }

    // process PO gate
    for (auto& gate : gates) {
        if (gate->type != PO) {
            continue;
        }
        std::string targetGateName = boost::replace_last_copy(gate->name, "_PO", "");
        Gate* targetGate = name2Gate[targetGateName];
        gate->inputs.push_back(targetGate);
        targetGate->outputs.push_back(gate);
    }

    for (int32_t gateId = 0; gateId < gates.size(); gateId++) {
        gates[gateId]->id = gateId;
    }

    SortGates();
    CheckFloating();

    CreateFaultlist();
}

void Netlist::SortGates()
{
    std::sort(gates.begin(), gates.end(), [&](Gate* gate1, Gate* gate2) {
        if (gate1->type < gate2->type) {
            return true;
        }

        return (gate1->name.compare(gate2->name) < 0);
    });

    for (auto gate : gates) {
        std::cout << gate->name << std::endl;
    }
}

void Netlist::CreateFaultlist() {
    FaultType faultType;
    for (auto gate : gates) {
        if (gate->inputs.size() > 1) {  // add s-a-1 for AND/NAND, add s-a-0 for OR/NOR, add both for other gate.
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
        if (gate->outputs.size() == 1 && (gate->outputs[0]->inputs.size() > 1 || gate->outputs[0]->type == PO)) {
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

    SaveFaultlist();
}

void Netlist::SaveFaultlist() {
    std::string faultlistFile = Params::GetInstance()->GetFaultlistFile();
    if (faultlistFile.empty()) {
        return;
    }

    std::ofstream faultlistFileName(faultlistFile);

    for (int32_t i = 0; i < faultlist.size(); i++) {
        faultlist[i]->id = i;
        faultlistFileName << faultlist[i]->type << " UC.UNK " << faultlist[i]->pinName << std::endl;

        // atalanta format for debug
        if (faultlist[i]->pin == 0) {
            std::cout << i << "\t" << faultlist[i]->gate->name << " /" << faultlist[i]->type << std::endl;
        } else {
            std::cout << i << "\t" << faultlist[i]->gate->inputs[faultlist[i]->pin - 1]->name << "->" << faultlist[i]->gate->name << " /" << faultlist[i]->type << std::endl;
        }
    }

    std::cout << "Save faultlist as " << faultlistFile << std::endl;
}

void Netlist::CheckFloating() {
    for (auto gate : gates) {
        if (gate->outputs.size() == 0 && gate->type != PO) {
            std::cerr << "floating gate: " << gate->name << std::endl;
        }
    }
}
