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
#ifndef R7_FAULTLIST_H
#define R7_FAULTLIST_H

#include <vector>

class Netlist;
struct Fault;
class Faultlist {
public:
    void Clean();

    int CreateFaults(Netlist* netlist);

    int DumpFaults(const std::string& faultlistFile);

    const std::vector<Fault*>& Faults() const
    {
        return faultlist;
    }

    std::vector<Fault*>& Faults()
    {
        return faultlist;
    }

private:
    std::vector<Fault*> faultlist; // ATPG需要target的所有的fault，已经做了故障折叠处理
};

#endif //R7_FAULTLIST_H
