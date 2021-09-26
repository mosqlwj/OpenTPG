/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "Statuslist.h"
#include "Faultlist.h"

int Statuslist::Create(Faultlist* faultlist) {
    ASSERT(faultlist != nullptr);
    ASSERT(faultlist->Faults().size() > 0);
    faultStatus.resize(faultlist->Faults().size(), FaultStatus::INIT);
    return 0;
}

void Statuslist::UpdateStatus(int32_t faultId, FaultStatus newStatus) {
    ASSERT(faultId >= 0);
    ASSERT(faultId < faultStatus.size());
    faultStatus[faultId] = newStatus;
}

void Statuslist::Clean() {
    auto size = faultStatus.size();
    faultStatus.clear();
    faultStatus.resize(size, FaultStatus::INIT);
}

FaultStatus Statuslist::operator[](int32_t faultId) const {
    ASSERT(faultId >= 0);
    ASSERT(faultId < faultStatus.size());
    return faultStatus[faultId];
}
