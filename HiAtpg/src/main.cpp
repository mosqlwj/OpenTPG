#include "Netlist.h"
#include "Params.h"

int main(int argc, char** argv) {
    auto params = Params::getInstance();
    params->parseCheck(argc, argv);
    Netlist* netlist = new Netlist();
    netlist->Parse(params->getNetlistFile());
    return 0;
}
