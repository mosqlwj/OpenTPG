//
// Created by fish on 2021/6/28.
//

#include "Netlist.h"
#include <string>
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include "Util.h"

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
        if (outputHead ==res[0]) {
            Gate* gate = new Gate(res[1], Util::GetGateTypeFromString(res[0]));
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

    std::sort(gates.begin(), gates.end(), [&](Gate* gate1, Gate* gate2) {
        return (gate1->type < gate2->type);
    });

    for (int32_t gateId = 0; gateId < gates.size(); gateId++) {
        gates[gateId]->id = gateId;
    }

    std::cout << "Parse OK" << std::endl;
}
