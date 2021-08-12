#include "ATPGDriverDefault.h"
#include "CubeHandlerWriteFile.h"
#include "Faultlist.h"
#include "Netlist.h"
#include "Params.h"
#include "printers.h"

int main(int argc, char** argv)
{
    auto params = Params::GetInstance();
    params->parseCheck(argc, argv);

    //  读取网表
    Netlist netlist;
    netlist.Parse(params->GetNetlistFile());

    //  生成fault
    Faultlist faultlist;

    //  生成cube
    std::unique_ptr<ATPGDriver> atpgDriver(CreateATPGDriver());
    CubeHandlerWriteFile cubeOutputPrinter(params->GetCubeDumpFile());
    atpgDriver->SetupCubeOutput(&cubeOutputPrinter);
    atpgDriver->SetupNetlist(&netlist);
    atpgDriver->SetupFaultlist(&faultlist);
    int ret = atpgDriver->Prepare();
    if (ret != 0) {
        return -1;
    }

    atpgDriver->Execute();

    return 0;
}
