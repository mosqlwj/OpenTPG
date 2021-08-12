#include "Netlist.h"
#include "Params.h"

int main(int argc, char** argv)
{
    auto params = Params::GetInstance();
    params->parseCheck(argc, argv);

    Netlist* netlist = new Netlist();
    netlist->Parse(params->GetNetlistFile());
    return 0;
}
