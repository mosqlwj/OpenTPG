//
// Created by luolijun on 2021/8/12.
//

#ifndef FAULTLIST_H
#define FAULTLIST_H

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

#endif //FAULTLIST_H
