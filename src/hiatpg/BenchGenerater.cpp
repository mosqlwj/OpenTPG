//
// Created by 50447 on 2020/7/4.
//
#include "model.h"
#include "BenchGenerater.h"
bool BenchGenerater::CombineBenchFile(const vector<string>& files, int times)
{
    vector<FlattenGates> gates;
    auto netlist = NetList::GetInstance();
    netlist->ClearGates();
    for (auto& file : files) {
         netlist->Parse(file);
         gates.push_back(netlist->GetGates());
         netlist->ClearGates();
    }
    vector<FlattenGates> divideGates(GROUP_SIZE);
    CombineGate(gates, divideGates);

    int sequence = times / 2;
    for (int i = 0; i< sequence; ++i) {
        vector<FlattenGates> inputs(2);
        int gateId = 0;
        for (auto& typeGates : divideGates) {
             for (auto& gate : typeGates) {
                gate->gateId = gateId++;
                inputs[0].push_back(gate);
             }
        }
        CopyFlattenGates(inputs);
        divideGates.clear();
        CombineGate(inputs, divideGates);
        std::cout << i << " finishded" << std::endl;
    }
    int gateId = 0;
    for (auto& typeGates : divideGates) {
         for (auto& gate : typeGates) {
            gate->gateId = gateId++;
         }
    }
    return SaveToFile(divideGates);
}

void BenchGenerater::CombineGate(vector<FlattenGates>& gates, vector<FlattenGates>& divideGates)
{
    if (divideGates.size() != GROUP_SIZE) {
        divideGates.resize(GROUP_SIZE);
    }
    for (auto& flattenGates : gates) {
         for (auto& gate : flattenGates) {
             switch (gate->type) {
                 case PI: {
                     divideGates[GROUP_PI].push_back(gate);
                     break;
                 }
                 case PO: {
                     divideGates[GROUP_PO].push_back(gate);
                     break;
                 }
                 default: {
                     divideGates[GROUP_COM].push_back(gate);
                     break;
                 }
             }
         }
    }
    FlattenGates poGates = divideGates[GROUP_PO];
    std::random_shuffle(poGates.begin(), poGates.end());
    divideGates[GROUP_PO].clear();
    int poSize = poGates.size();
    for (int i = 0; i < poSize; ++i) {
        if (i + 1 < poSize) {
            Gate* andGate = new Gate(-1, AND, "");
            divideGates[GROUP_CONNPO].push_back(andGate);
            Gate* buf1 = poGates[i];
            buf1->type = BUF;
            buf1->fanouts.push_back(andGate);
            divideGates[GROUP_COM].push_back(buf1);
            i++;
            Gate* buf2 = poGates[i];
            buf2->type = BUF;
            buf2->fanouts.push_back(andGate);
            divideGates[GROUP_COM].push_back(buf2);
            andGate->fanins.push_back(buf1);
            andGate->fanins.push_back(buf2);
            Gate* poGate = new Gate(-1, PO, "");
            poGate->fanins.push_back(andGate);
            andGate->fanouts.push_back(poGate);
            divideGates[GROUP_PO].push_back(poGate);
        } else {
            Gate* poGate = new Gate(-1, PO, "");
            Gate* buf1 = poGates[i];
            buf1->type = BUF;
            buf1->fanouts.push_back(poGate);
            divideGates[GROUP_CONNPO].push_back(buf1);
            poGate->fanins.push_back(buf1);
            divideGates[GROUP_PO].push_back(poGate);
        }
    }
}

void BenchGenerater::CopyFlattenGates(vector<FlattenGates> &gates)
{
    FlattenGates& srcGates = gates[0];
    FlattenGates& tarGates = gates[1];
    for (auto& gate : srcGates) {
        Gate* newGate = new Gate(gate->gateId, gate->type, gate->name);
        tarGates.push_back(newGate);
    }

    int gateSize = srcGates.size();
    for (int i = 0; i < gateSize; ++i) {
        Gate* gate = srcGates[i];
        Gate* tarGate = tarGates[i];
        for (auto& fin : gate->fanins) {
            tarGate->fanins.push_back(tarGates[fin->gateId]);
        }
        for (auto& fout : gate->fanouts) {
            tarGate->fanouts.push_back(tarGates[fout->gateId]);
        }
    }
}

bool BenchGenerater::SaveToFile(vector<FlattenGates>& gates)
{
    std::cout << "Begin to save file." <<std::endl;
    string path = "./" + fileName;
    ofstream file(path, std::ios::out);
    if (!file.is_open()) {
        std::cout << "open combine.bench failed\n";
        return false;
    }
    int comSize = 0;
    for (int i = AND; i< gates.size(); ++i) {
        comSize += gates[i].size();
    }

    //file << "# " + fileName + "\n";
    //file << "# " << gates[PI].size() << " inputs\n";
    //file << "# " << gates[PO].size() << " outputs\n";
    //file << "# " << comSize << " gates\n";
    for (auto& gate : gates[GROUP_PI]) {
        file << "INPUT(" << gate->gateId << ")\n";
    }
    file << "\n";
    for (auto& gate : gates[GROUP_PO]) {
        file << "OUTPUT(" << gate->gateId << ")\n";
    }
    file << "\n";
    WriteLogicGates(gates[GROUP_COM], file);
//    WriteLogicGates(gates[NAND], file, "NAND");
//    WriteLogicGates(gates[OR], file, "OR");
//    WriteLogicGates(gates[NOR], file, "NOR");
//    WriteLogicGates(gates[XOR], file, "XOR");
//    WriteLogicGates(gates[XNOR], file, "XNOR");
//    WriteLogicGates(gates[BUF], file, "BUF");
//    WriteLogicGates(gates[INV], file, "INV");
    file << "\n";
    for (auto& gate : gates[GROUP_CONNPO]) {
        Gate* poGate = gate->fanouts[0];
        file << poGate->gateId << "=" << gateType2Str[gate->type] << "(";
        bool firstFain = true;
        for (auto& fin : gate->fanins) {
            if (firstFain) {
                file << fin->gateId;
                firstFain = false;
            } else {
                file << "," << fin->gateId;
            }
        }
        file << ")\n";
    }
    file.close();
    std::cout << "Save finished." <<std::endl;
    return true;
}

void BenchGenerater::WriteLogicGates(const vector<Gate*>& gates, ofstream& ofile)
{
    for (auto& gate : gates) {
        ofile << gate->gateId << "=" << gateType2Str[gate->type] << "(";
        bool firstFain = true;
        for (auto& fin : gate->fanins) {
            if (firstFain) {
                ofile << fin->gateId;
                firstFain = false;
            } else {
                ofile << "," << fin->gateId;
            }
        }
        ofile << ")\n";
    }
}


