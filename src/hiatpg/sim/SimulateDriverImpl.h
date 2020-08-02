//
// Created by luolijun on 2020/7/15.
//

#ifndef OPENTPG_SIMULATEDRIVERIMPL_H
#define OPENTPG_SIMULATEDRIVERIMPL_H

#include "HIATPG.h"



class SimulateDriverImpl : public SimulateDriver
{
public:
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, const FaultListTable* faultlist, Options* options) override;

    //! 设置输出
    virtual int Setup(CubeOutput* output, FaultMaker* marker) override;

    //! 执行仿真
    virtual int Simulate(Pattern* pattern) override;
};



#endif //OPENTPG_SIMULATEDRIVERIMPL_H
