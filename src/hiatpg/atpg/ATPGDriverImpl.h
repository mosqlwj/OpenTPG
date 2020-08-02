//
// Created by luolijun on 2020/7/15.
//

#ifndef OPENTPG_ATPGDRIVERIMPL_H
#define OPENTPG_ATPGDRIVERIMPL_H


#include "HIATPG.h"



class ATPGDriverImpl : public ATPGDriver
{
public:
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, Options* options) override;

    //! 设置输出
    virtual int Setup(CubeOutput* output) override;

    //! 执行探测
    virtual int Detect(Fault* fault) override;
};

class AtpgEngine
{
    std::unordered_map<GateId, AtpgValue> window;

    std::unordered_map<GateId, Value> testCube;
};



#endif //OPENTPG_ATPGDRIVERIMPL_H
