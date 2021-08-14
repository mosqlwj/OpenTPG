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
#include "Fault.h"

static std::vector<std::string> normalGatePinName = { "Z", "I1", "I2", "I3", "I4" };
static std::vector<std::string> dffGatePinName = { "Q", "D", "CK" };

std::string Fault::GetFaultName(const Gate* gate, int32_t pinIdx)
{
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
