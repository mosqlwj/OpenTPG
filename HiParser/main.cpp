#include <iostream>
#include "Netlist.h"

int main() {
    Netlist* netlist = new Netlist();
    std::string netlistPath = "../bench/s27.bench";
    netlist->Parse(netlistPath);
    return 0;
}
