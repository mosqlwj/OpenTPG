//
// Created by luolijun on 2020/7/15.
//

#ifndef OPENTPG_NETLISTTABLEIMPL_H
#define OPENTPG_NETLISTTABLEIMPL_H

#include "HIATPG.h"

#include "line_reader.h"





class NetListTableImpl : public NetListTable
{
public:
    //! 从指定的 reader 读取网表信息
    virtual int Load(Reader& reader) override;

    //! 将网表信息保存到文件
    virtual int Save(Writer& writer) override;

    //! 根据gate的名字找到Gate对象
    virtual const Gate* GateOf(const std::string& name) const override;

    //! 根据gate的名字找到Gate对象
    virtual Gate* GateOf(const std::string& name) override;

    //! 根据gate的id找到Gate对象
    virtual const Gate* GateOf(GateId gateId) const override;

    //! 根据gate的id找到Gate对象
    virtual Gate* GateOf(GateId gateId) override;

    //! 获取所有的Gate
    virtual const std::vector<const Gate*>& GetGates() const override;

    //! 获取所有的Gate
    virtual std::vector<Gate*>& GetGates() override;

    //! 枚举所有的Gate
    virtual void Access(std::function<int(Gate*)> handler) override;

    //! 枚举所有的Gate
    virtual void Access(std::function<int(const Gate*)> handler) const override;

    //! 对象清理
    virtual void Clear() override;


private:
    std::vector<Gate*> gates;

    std::unordered_map<std::string, Gate*> name2GatePointer;
};



#endif //OPENTPG_NETLISTTABLEIMPL_H
