//
// Created by luolijun on 2021/8/13.
//

#ifndef CONTEXTDEFAULT_H
#define CONTEXTDEFAULT_H

class Netlist;
class Faultlist;
class Params;

struct ContextDefault {
    Netlist* netlist;
    Faultlist* faultlist;
    Params* params;
};

#endif //CONTEXTDEFAULT_H
