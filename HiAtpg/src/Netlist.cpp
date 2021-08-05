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
#include "ScanChain.h"

void Netlist::Parse(const std::string &fileName) {
    std::string line;
    const std::string commentHead = "#";
    const std::string inputHead = "INPUT";
    const std::string outputHead = "OUTPUT";

    std::ifstream netlist(fileName);
    if (!netlist.good()) {
        std::cerr << "Can not find netlist file: " << fileName << std::endl;
    }
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
            std::string poGateName = res[1] + "_PO";
            Gate* gate = new Gate(poGateName, Util::GetGateTypeFromString(res[0]));
            gates.push_back(gate);
            name2Gate[poGateName] = gate;
            numOfPO++;
            continue;
        }

        // eg: U1 = AND(U2, U3);
        GateType type = Util::GetGateTypeFromString(res[1]);
        if (type == DFF) {
            numOfDFF++;
        }
        Gate* gate = new Gate(res[0], type);
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

    SortGates();
    TagGateRange();
    ReOrderDff();
    DumpGates();
    CheckFloating();
    CreateFaultlist();
    bool ret = ParseConfig();
    if (!ret) {
        std::cerr << "Parse Failed!" << std::endl;
    }
}

void Netlist::SortGates()
{
    std::sort(gates.begin(), gates.end(), [&](Gate* gate1, Gate* gate2) {
        if (gate1->type < gate2->type) {
            return true;
        } else if (gate1->type > gate2->type) {
            return false;
        }

        return (gate1->name < gate2->name);
    });

    for (int32_t gateId = 0; gateId < gates.size(); gateId++) {
        gates[gateId]->id = gateId;
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
    auto& faultlistFile = Params::GetInstance()->GetFaultlistFile();
    if (faultlistFile.empty()) {
        return;
    }

    std::ofstream faultlistFileName(faultlistFile);

    for (int32_t i = 0; i < faultlist.size(); i++) {
        faultlist[i]->id = i;
        faultlistFileName << faultlist[i]->type << " UC.UNK " << faultlist[i]->pinName << std::endl;

        // atalanta format for debug
//        if (faultlist[i]->pin == 0) {
//            std::cout << i << "\t" << faultlist[i]->gate->name << " /" << faultlist[i]->type << std::endl;
//        } else {
//            std::cout << i << "\t" << faultlist[i]->gate->inputs[faultlist[i]->pin - 1]->name << "->" << faultlist[i]->gate->name << " /" << faultlist[i]->type << std::endl;
//        }
    }

    std::cout << "Save faultlist as " << faultlistFile << std::endl;
}

void Netlist::CheckFloating() {
    for (auto gate : gates) {
        if (gate->outputs.empty() && gate->type != PO) {
            std::cerr << "floating gate: " << gate->name << std::endl;
        }
    }
}

bool Netlist::ParseConfig() {
    std::string configFileName = Params::GetInstance()->GetConfigFile();

    std::string line;
    std::ifstream configFile(configFileName);

    if (!configFile.good()) {
        std::cerr << "Can not find config file: " << configFileName << std::endl;
        return false;
    }

    // 1st read by line, parse all gate
    enum ParseStat {IDLE, SCAN_CHAIN, CLOCK};
    ParseStat parseStat = IDLE;
    while (getline(configFile, line)) {
        if (line.empty()) {
            continue;
        }

        if (line.find("scan chain") != std::string::npos) {
            parseStat = SCAN_CHAIN;
            continue;
        } else if (line.find("clock") != std::string::npos) {
            parseStat = CLOCK;
            continue;
        }

        std::vector<std::string> res;
        boost::split(res, line, boost::is_any_of("{} "), boost::token_compress_on);
        switch (parseStat) {
            case IDLE:
                break;
            case SCAN_CHAIN: {
                //        for (auto r : res) {
                //            std::cout << r << std::endl;
                //        }
                std::string chainName = res[0];
                Gate* siGate = name2Gate[res[1]];
                Gate* soGate = name2Gate[res[2]];
                ScanChain* scanChain = new ScanChain(chainName, siGate, soGate);
                scanChain->Trace(name2Gate);
                scanChains.push_back(scanChain);
                break;
            }
            case CLOCK: {
                std::string clockName = res[0];
                Gate* clockGate = name2Gate[clockName];
                LogicVal offVal = static_cast<LogicVal>(std::atoi(res[1].c_str()) + LOGIC_0);
                clk2OffVal[clockGate] = offVal;
                break;
            }
            default:
                break;
        }
    }

    return true;
}

void Netlist::TagGateRange() {
    dffBegin = (*std::find_if(gates.begin(), gates.end(), [](Gate* gate) { return gate->type == DFF; }))->id;
    dffEnd = dffBegin + numOfDFF;
}

void Netlist::ReOrderDff() {
    std::sort(gates.begin() + dffBegin, gates.begin() + dffEnd, [&](Gate* gate1, Gate* gate2) {
        assert(gate1->inputs.size() == 2);
        return (gate1->inputs[1]->type == MUX && gate2->inputs[1]->type != MUX);
    });

    for (GateId gateId = dffBegin; gateId < dffEnd; gateId++) {
        gates[gateId]->id = gateId;
//        std::cout << gates[gateId]->name << std::endl;
    }
}

void Netlist::DumpGates() {
    std::string gateDumpFileName = Params::GetInstance()->GetGateDumpFile();
    if (gateDumpFileName.empty()) {
        return;
    }
    std::ofstream gateDumpFile(gateDumpFileName);

    for (auto gate : gates) {
        if (gate->type == DFF && gate->inputs[1]->type != MUX) {
            break;
        }
        gateDumpFile << gate->name << std::endl;
    }

    std::cout << "Save gates as " << gateDumpFileName << std::endl;
}
