#include "ATPGDriverDefault.h"
#include "ContextDefault.h"
#include "CubeHandlerWriteFile.h"
#include "Faultlist.h"
#include "Netlist.h"
#include "Params.h"
#include "errors.h"

int main(int argc, char** argv)
{
    Params* params = Params::GetInstance();
    params->parseCheck(argc, argv);

    //  读取网表
    Netlist netlist;
    int ret = netlist.LoadNetlist(params->GetNetlistFile(), params->GetConfigFile());
    if (ret != 0) {
        std::cerr << "Load netlist failed. netlist=" << params->GetNetlistFile() << ", config=" << params->GetConfigFile() << std::endl;
        return errors::LOAD_NETLIST_FAILED;
    }

    //  生成fault
    Faultlist faultlist;
    ret = faultlist.CreateFaults(&netlist);
    if (ret != 0) {
        std::cerr << "Create faultlist failed." << std::endl;
        return errors::CREATE_FAULTLIST_FAILED;
    }

    //  定义一个缺省的 context 对象
    ContextDefault context;
    context.netlist = &netlist;
    context.faultlist = &faultlist;
    context.params = params;

    //  生成cube
    std::unique_ptr<ATPGDriver> atpgDriver(CreateATPGDriver(&context));
    CubeHandlerWriteFile cubeOutputPrinter(params->GetCubeDumpFile());
    atpgDriver->SetupCubeOutput(&cubeOutputPrinter);
    atpgDriver->SetupNetlist(&netlist);
    atpgDriver->SetupFaultlist(&faultlist);
    ret = atpgDriver->Prepare();
    if (ret != 0) {
        return errors::ATPG_PREPARE_FAILED;
    }

    atpgDriver->Execute();
    netlist.SaveFaultlist();

    return 0;
}
