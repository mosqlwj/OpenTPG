//
// Created by luolijun on 2020/7/15.
//

#ifndef OPENTPG_FAULTLISTTABLEIMPL_H
#define OPENTPG_FAULTLISTTABLEIMPL_H


#include "HIATPG.h"



class FaultListTableImpl : public FaultListTable
{
private:
    std::vector<Fault*> faults;

public:
    virtual int Setup(const NetListTable* netlist) override;

    //! 根据网表创建 FaultList
    virtual int Create() override;

    //! 从指定的 reader 流加载 FaultList
    virtual int Load(Reader& reader) override;

    //! 将所有的 Fault 写入指定的Writter
    virtual int Save(Writer& writer) override;

    //! 更新指定的 Fault 的状态
    virtual void UpdateFaultStatus(FaultStatus status) override;

    //! 获取 Fault 列表
    virtual const std::vector<const Fault*> GetFaults() const override;

    //! 获取 Fault 列表
    virtual std::vector<Fault*> GetFaults() override;

    //! 枚举 Fault 列表
    virtual void Access(std::function<int(Fault*)> handler) override;

    //! 枚举 Fault 列表
    virtual void Access(std::function<int(const Fault*)> handler) const = 0;

    virtual void Clear() override;

private:
    const NetListTable* netlist;
};



#endif //OPENTPG_FAULTLISTTABLEIMPL_H
