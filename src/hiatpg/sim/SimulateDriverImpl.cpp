//
// Created by luolijun on 2020/7/15.
//

#include "SimulateDriverImpl.h"



IMPLEMENT_MODULE(ModuleID::MODULE_SIMULATEDRIVER, SimulateDriverImpl, "default");



//! 设置输入
int SimulateDriverImpl::Setup(int id, const NetListTable* netlist, const FaultListTable* faultlist, Options* options)
{
    return -1;
}

//! 设置输出
int SimulateDriverImpl::Setup(CubeOutput* output, FaultMaker* marker)
{
    return -1;
}


//! 执行仿真
int SimulateDriverImpl::Simulate(Pattern* pattern)
{
    return -1;
}
