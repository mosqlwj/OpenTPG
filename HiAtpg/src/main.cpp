#include "ATPGDriverDefault.h"
#include "CubeHandlerWriteFile.h"
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

    //  生成cube
    std::unique_ptr<ATPGDriver> atpgDriver(CreateATPGDriver());
    CubeHandlerWriteFile cubeOutputPrinter(params->GetCubeDumpFile());
    atpgDriver->SetupCubeOutput(&cubeOutputPrinter);
    atpgDriver->SetupNetlist(&netlist);
    int ret = atpgDriver->Prepare();
    if (ret != 0) {
        return -1;
    }

    atpgDriver->Execute();
    netlist.SaveFaultlist();

    return 0;
}
