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
#ifndef OPENTPG_CONTEXTDEFAULT_H
#define OPENTPG_CONTEXTDEFAULT_H

class Netlist;
class Faultlist;
class Statuslist;
class Params;

struct ContextDefault {
    Netlist* netlist { nullptr };
    Faultlist* faultlist { nullptr };
    Statuslist* statuslist { nullptr };
    Params* params { nullptr };
};

#endif //R7_CONTEXTDEFAULT_H
