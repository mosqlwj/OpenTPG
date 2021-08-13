#include "ATPGDriverDefault.h"
#include "ContextDefault.h"
#include "CubeHandlerWriteFile.h"
#include "Faultlist.h"
#include "Netlist.h"
#include "Params.h"
#include "errors.h"

int main(int argc, char** argv)
{
    //  读取用户配置
    Params& params = Params::GetInstance();
    params.Load(argc, argv);

    //  读取网表
    Netlist netlist;
    int ret = netlist.LoadNetlist(params.GetNetlistFile(), params.GetConfigFile());
    if (ret != 0) {
        std::cerr << "Load netlist failed. netlist=" << params.GetNetlistFile() << ", config=" << params.GetConfigFile() << std::endl;
        return errors::LOAD_NETLIST_FAILED;
    }

    //  如果指定了自动生成网表过程
    if (!params.GetGateDumpFile().empty()) {
        netlist.DumpGates(params.GetGateDumpFile());
    }

    //  生成 fault
    Faultlist faultlist;
    ret = faultlist.CreateFaults(&netlist);
    if (ret != 0) {
        std::cerr << "Create faultlist failed." << std::endl;
        return errors::CREATE_FAULTLIST_FAILED;
    }

    //  如果指定了 fault 需要写入文件
    if (!params.GetFaultlistFile().empty()) {
        faultlist.DumpFaults(params.GetFaultlistFile());
    }

    //  定义一个缺省的 context 对象
    ContextDefault context;
    context.netlist = &netlist;
    context.faultlist = &faultlist;
    context.params = &params;

    //  生成 cube
    std::unique_ptr<ATPGDriver> atpgDriver(CreateATPGDriver(&context));
    CubeHandlerWriteFile cubeOutputPrinter(params.GetCubeDumpFile());
    atpgDriver->SetupCubeOutput(&cubeOutputPrinter);
    atpgDriver->SetupNetlist(&netlist);
    atpgDriver->SetupFaultlist(&faultlist);
    ret = atpgDriver->Prepare();
    if (ret != 0) {
        return errors::ATPG_PREPARE_FAILED;
    }

    atpgDriver->Execute();

    return 0;
}
