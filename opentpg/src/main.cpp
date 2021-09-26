/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "ATPGDriverDefault.h"
#include "ContextDefault.h"
#include "CubeHandlerWriteFile.h"
#include "Faultlist.h"
#include "Netlist.h"
#include "Params.h"
#include "SimulatorDefault.h"
#include "Statuslist.h"
#include "errors.h"

#include <memory>

int main(int argc, char** argv) {
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

    //  生成状态表
    Statuslist statuslist;
    ret = statuslist.Create(&faultlist);
    ASSERT(ret == 0);

    //  定义一个缺省的 context 对象
    ContextDefault context;
    context.netlist = &netlist;
    context.faultlist = &faultlist;
    context.statuslist = &statuslist;
    context.params = &params;

    //  定义一个仿真器
    SimulatorDefault simulator;
    simulator.SetupNetlist(&netlist);

    //  生成 cube
    std::unique_ptr<ATPGDriver> atpgDriver(CreateATPGDriver(&context));
    CubeHandlerWriteFile cubeOutputPrinter(params.GetCubeDumpFile(), &simulator);
    atpgDriver->SetupCubeOutput(&cubeOutputPrinter);
    atpgDriver->SetupNetlist(&netlist);
    atpgDriver->SetupFaultlist(&faultlist);
    ret = atpgDriver->Prepare();
    if (ret != 0) {
        return errors::ATPG_PREPARE_FAILED;
    }

    //  执行计算
    atpgDriver->Execute();

    //  清理
    atpgDriver->Cleanup();

    //  如果指定了 fault 需要写入文件
    if (!params.GetFaultlistFile().empty()) {
        faultlist.DumpFaults(params.GetFaultlistFile(), statuslist);
    }

    return 0;
}
