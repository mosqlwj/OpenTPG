//
// Created by luolijun on 2020/7/15.
//

#include "ATPGDriverImpl.h"



IMPLEMENT_MODULE(ModuleID::MODULE_ATPGDRIVER, ATPGDriverImpl, "default");



//! 设置输入
int ATPGDriverImpl::Setup(int id, const NetListTable* netlist, Options* options)
{
    return -1;
}


//! 设置输出
int ATPGDriverImpl::Setup(CubeOutput* output)
{
    return -1;
}


//! 执行探测
int ATPGDriverImpl::Detect(Fault* fault)
{
    return -1;
}

