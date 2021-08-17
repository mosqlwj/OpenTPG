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
#ifndef OPENTPG_FAULTLISTSTATUS_H
#define OPENTPG_FAULTLISTSTATUS_H

#include <vector>

#include "common.h"

class Faultlist;

class Statuslist {
public:
    int Create(Faultlist* faultlist);
    void UpdateStatus(int32_t faultId, FaultStatus newStatus);
    void Clean();
    FaultStatus operator[](int32_t faultId) const;

private:
    std::vector<FaultStatus> faultStatus;
};

#endif //FAULTLISTSTATUS_H
