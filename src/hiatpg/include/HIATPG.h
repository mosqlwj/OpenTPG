#ifndef HIATPG_H
#define HIATPG_H

#include <string>
#include <vector>

#include "model.h"



struct Interface
{
    virtual ~Interface()
    {}
};



typedef Interface* (* ModuleCreator)();


enum
{
    MODULE_COUNT_MAX = 10,
    MODULE_NAME_LEN_MAX = 11,
};



struct ModuleDefinition
{
    ModuleDefinition* next;

    ModuleDefinition* prev;

    char name[MODULE_NAME_LEN_MAX + 1];

    ModuleCreator creator;

    ModuleDefinition()
    {
        next = this;
        prev = this;
        name[0] = '\0';
        creator = nullptr;
    }
};



//! 定义一个模块
extern int DefineModule(int mid, ModuleDefinition* def);

//! 创建一个模块实例
extern int CreateModule(int mid, const char* name);

//! 根据 mid 引用某个模块
extern Interface* ReferModule(int mid);

//! 实现某个模块,实现某个模块之后,该模块就可以被其他模块引用
#define IMPLEMENT_MODULE(MID, IMPL, IMPLNAME)                \
    class IMPL##Creator : public ModuleDefinition {          \
    public:                                                  \
        IMPL##Creator() {                                    \
            strncpy(name, IMPLNAME, sizeof(name));           \
            creator = IMPL##Creator::CreateModule;           \
            ::DefineModule(MID, this);                       \
        }                                                    \
        static Module* CreateModule() { return new IMPL(); } \
    } IMPL##CreatorInstance



//! Writer 对象用于支持以流的形式写入数据
struct Writer : virtual public Interface
{
    //! 写入数据
    virtual int64_t Write(const uint8_t* data, int64_t) = 0;
};



//! Reader 对象用于支持按照流的形式读取数据
struct Reader : virtual public Interface
{
    //! 读取数据
    virtual int64_t Read(const uint8_t* data, int64_t) = 0;
};



//! Clearable 支持清理机制,调用 Clear 函数后,对象内部的数据会被自动清理掉
struct Clearable : virtual public Interface
{
    //! 对对象的内部状态进行清理
    virtual void Clear() = 0;
};



//! 全局的清理管理机制,用于支持批量清理对象
struct ClearManager : virtual public Clearable
{
    //! 注册一个自动清理器
    virtual void Register(Clearable* object) = 0;

    //! 注销一个自动清理器
    virtual void Unregister(Clearable* object) = 0;
};



struct Gate;



//  网表操作接口
struct NetListTable : virtual public Interface
{
    //! 从指定的 reader 读取网表信息
    virtual int Load(Reader& reader) = 0;

    //! 将网表信息保存到文件
    virtual int Save(Writer& writer) = 0;

    //! 根据gate的名字找到Gate对象
    virtual const Gate* GateOf(const std::string& name) const = 0;

    //! 根据gate的名字找到Gate对象
    virtual Gate* GateOf(const std::string& name) = 0;

    //! 根据gate的id找到Gate对象
    virtual const Gate* GateOf(GateId gateId) const = 0;

    //! 根据gate的id找到Gate对象
    virtual Gate* GateOf(GateId gateId) = 0;

    //! 获取所有的Gate
    virtual const std::vector<const Gate*>& GetGates() const = 0;

    //! 获取所有的Gate
    virtual std::vector<Gate*>& GetGates() = 0;

    //! 枚举所有的Gate
    virtual void Access(std::function<int(Gate*)> handler) = 0;

    //! 枚举所有的Gate
    virtual void Access(std::function<int(const Gate*)> handler) const = 0;
};



//! Fault 表
struct FaultListTable : virtual public Clearable
{
    //! 根据网表创建 FaultList
    virtual int Create(const NetListTable& netlist) = 0;

    //! 从指定的 reader 流加载 FaultList
    virtual int Load(Reader& reader) = 0;

    //! 将所有的 Fault 写入指定的Writter
    virtual int Save(Writer& writer) = 0;

    //! 更新指定的 Fault 的状态
    virtual void UpdateFaultStatus(FaultStatus status) = 0;

    //! 获取 Fault 列表
    virtual const vector<const Fault*> GetFaults() const = 0;

    //! 获取 Fault 列表
    virtual vector<Fault*> GetFaults() = 0;

    //! 枚举 Fault 列表
    virtual void Access(std::function<int(Fault*)> handler) = 0;

    //! 枚举 Fault 列表
    virtual void Access(std::function<int(const Fault*)> handler) const = 0;
};



struct Cube;



//! Cube 输出接口
struct CubeOutput : virtual public Interface
{
    //! 当 Cube 需要被输出时,会调用该接口
    virtual int Handle(Cube* cube) = 0;
};



//! Pattern 输出接口
struct PatternOutput : virtual public Interface
{
    //! 当 Pattern 需要被输出时,会调用该接口
    virtual int Handle(Pattern* pattern) = 0;
};



//! Fault 状态标记
struct FaultMaker : virtual public Interface
{
    //! 更新 Fault 的状态
    virtual void UpdateFaultStatus(int32_t faultIndex, FaultStatus status) = 0;
};



//! ATPG 驱动
struct ATPGDriver : virtual public Clearable
{
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, Options* options) = 0;

    //! 设置输出
    virtual int Setup(CubeOutput* output) = 0;

    //! 执行探测
    virtual int Detect(Fault* fault) = 0;
};



//! 仿真驱动
struct SimulateDriver : virtual public Clearable
{
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, const FaultListTable* faultlist, Options* options) = 0;

    //! 设置输出
    virtual int Setup(CubeOutput* output, FaultMaker* marker) = 0;

    //! 执行仿真
    virtual int Simulate(Pattern* pattern) = 0;
};



//!  用于写 Pattern
struct PatternWriter
{
    //! 设置输出
    virtual int Setup(Writer* w) = 0;

    //! 指定处理Pattern
    virtual int Handle(const Pattern* p) = 0;

    //! 当Pattern写入完成之后,可提交,以完成首尾工作
    virtual int Commit() = 0;
};



//! Pattern表的管理
struct PatternListTable : virtual public Clearable
{
    //! 枚举所有的 Pattern
    virtual void Access(std::function<int(Pattern*)> handler) = 0;

    //! 枚举所有的 Pattern
    virtual void Access(std::function<int(const Pattern*)> handler) = 0;
};



#endif  // HIATPG_H
