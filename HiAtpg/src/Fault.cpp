//
// Created by fish on 2021/7/6.
//

#include "Fault.h"

static std::vector<std::string> normalGatePinName = {"Z", "I1", "I2", "I3", "I4"};
static std::vector<std::string> dffGatePinName = {"Q", "D", "CK"};

std::string Fault::GetFaultName(const Gate *gate, int32_t pinIdx) {
    std::string faultName = gate->name;
    switch (gate->type) {
        case DFF:
            faultName += ("/" + dffGatePinName[pinIdx]);
        case PI:
        case PO:
            break;
        default:
            faultName += ("/" + normalGatePinName[pinIdx]);
            break;
    }

    return faultName;
}
