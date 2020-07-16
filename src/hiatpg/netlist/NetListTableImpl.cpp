//
// Created by luolijun on 2020/7/15.
//

#include "NetListTableImpl.h"
#include "AssertUtils.h"


int NetListTableImpl::Load(Reader& reader)
{
//    std::ifstream in(fileName);
    GateId curGateId = 0;
    std::unordered_map<std::string, Gate*> primaryOutput;
//    string line;

    const uint8_t* text = nullptr;
    int64_t textLen = 0;
    while (0 == reader.ReadLine(text, textLen))
    {
        if (textLen == 0)
        {
            continue;
        }

        std::string line = (const char*) text;
        StringUtils::trim(line);

        std::vector<std::string> paras;
        paras = StringUtils::split(line, "[=,()]");
        Gate* gate;

        if (paras[0] == "INPUT")
        {
            auto name = paras[1];
            gate = new Gate(curGateId, PI, name);
            name2GatePointer[name] = gate;
        }
        else if (paras[0] == "OUTPUT")
        {
            auto name = paras[1];
            gate = new Gate(curGateId, PO, name);
            primaryOutput[name] = gate;
        }
        else
        {
            auto name = paras[0];
            GateType type = ::GateTypeOf(paras[1].c_str());
            gate = new Gate(curGateId, type, name);
            name2GatePointer[name] = gate;
            // add fanins
            for (int i = 2; i < paras.size(); ++i)
            {
                auto fanin = name2GatePointer[paras[i]];
                gate->fanins.push_back(fanin);
                fanin->fanouts.push_back(gate);
            }
        }
        gates.push_back(gate);
        curGateId++;
    }

    for (auto& po : primaryOutput)
    {
        auto name = po.first;
        auto gate = po.second;
        Gate* fanin = name2GatePointer[name];
        gate->fanins.push_back(fanin);
        fanin->fanouts.push_back(gate);
    }
    for (auto& po : primaryOutput)
    {
        auto name = po.first;
        auto gate = po.second;
        gate->name = name + "_PO";
        name2GatePointer[gate->name] = gate;
    }

    return 0;
}

int NetListTableImpl::Save(Writer& writer)
{
    for (auto gate : gates)
    {
        std::cout << "GateId: " << gate->gateId << std::endl;
        std::cout << "GateName: " << gate->name << std::endl;
        std::cout << "GateType: " << gate->type << std::endl;

        for (auto fanin : gate->fanins)
        {
            std::cout << "Fanin: " << fanin->name << std::endl;
        }
        for (auto fanout : gate->fanouts)
        {
            std::cout << "Fanout: " << fanout->name << std::endl;
        }
        std::cout << std::endl;
    }
}

const Gate* NetListTableImpl::GateOf(const std::string& name) const
{
    auto itr = name2GatePointer.find(name);
    if (name2GatePointer.end() ==  itr) {
        ASSERT(false);
        return nullptr;
    }

    return itr->second;
}

Gate* NetListTableImpl::GateOf(const std::string& name)
{
    auto itr = name2GatePointer.find(name);
    if (name2GatePointer.end() ==  itr) {
        ASSERT(false);
        return nullptr;
    }

    return itr->second;
}

std::vector<Gate*>& NetListTableImpl::GetGates()
{
    return gates;
}

const std::vector<const Gate*>& NetListTableImpl::GetGates() const
{
    return reinterpret_cast<const std::vector<const Gate*>&>(gates);
}


//! 枚举所有的Gate
void NetListTableImpl::Access(std::function<int(Gate*)> handler)
{
    for (auto gate : gates)
    {
        if (0 != handler(gate))
        {
            break;
        }
    }
}


//! 枚举所有的Gate
void NetListTableImpl::Access(std::function<int(const Gate*)> handler) const
{
    for (auto gate : gates)
    {
        if (0 != handler(gate))
        {
            break;
        }
    }
}


void NetListTableImpl::Clear()
{
    gates.clear();  //TODO 内存泄露
    name2GatePointer.clear();
}


