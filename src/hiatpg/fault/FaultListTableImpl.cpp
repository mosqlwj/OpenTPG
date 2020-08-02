//
// Created by luolijun on 2020/7/15.
//

#include "FaultListTableImpl.h"
#include "AssertUtils.h"
#include "StringUtils.h"

IMPLEMENT_MODULE(ModuleID::MODULE_FAULTLIST, FaultListTableImpl, "default");

int FaultListTableImpl::Setup(const NetListTable* n)
{
    ASSERT(n != nullptr);
    netlist = n;
    return 0;
}

int FaultListTableImpl::Create()
{
    return -1;
}


int FaultListTableImpl::Load(Reader& reader)
{
    int curFaultIndex = 0;

    const uint8_t* text = nullptr;
    int64_t textLen = 0;
    while (0 == reader.ReadLine(text, textLen))
    {
        if (textLen == 0)
        {
            continue;
        }

        std::string line((const char*) (text));

        std::vector<std::string> paras;
        paras = StringUtils::split(line, " ");

        auto gateName = paras[0];
        const Gate* gate = netlist->GateOf(gateName);
        Value value = (Value) (stoi(paras[1]) + ZERO);
        Fault* fault = new Fault(gate->gateId, curFaultIndex, value);
        faults.push_back(fault);
        curFaultIndex++;
    }

    return 0;
}

int FaultListTableImpl::Save(Writer& writer)
{
    for (auto fault : faults)
    {
        std::cout << "Fault gate id: " << fault->gateId << std::endl;
        std::cout << "Fault value: " << fault->value - ZERO << std::endl;
    }
}