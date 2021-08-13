//
// Created by luolijun on 2021/8/13.
//

#ifndef ATPGDRIVER_H
#define ATPGDRIVER_H

class Netlist;
class CubeHandler;
class Faultlist;
class ATPGDriver {
public:
    //! 析构函数
    virtual ~ATPGDriver() = default;

    //! 配置网表
    virtual void SetupNetlist(Netlist* n) = 0;

    //! 配置 Fault 表
    virtual void SetupFaultlist(Faultlist* f) = 0;

    //! 指定 Cube 输出目标
    virtual void SetupCubeOutput(CubeHandler* cubeOutput) = 0;

    //! 生成 cube 之前的准备工作
    virtual int Prepare() = 0;

    //! 执行生成 cube 流程
    virtual void Execute() = 0;

    //! 清理内部资源，使得整个驱动回归初始状态
    virtual void Cleanup() = 0;
};

extern ATPGDriver* CreateATPGDriver(void* context);

#endif //ATPGDRIVER_H
