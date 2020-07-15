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
    virtual int64_t Write(const uint8_t* data, int64_t) = 0;
};



//! Reader 对象用于支持按照流的形式读取数据
struct Reader : virtual public Interface
{
    virtual int64_t Read(const uint8_t* data, int64_t) = 0;
};



//! Clearable 支持清理机制,调用 Clear 函数后,对象内部的数据会被自动清理掉
struct Clearable : virtual public Interface
{
    virtual void Clear() = 0;
};



//! 全局的清理管理机制,用于支持批量清理对象
struct ClearManager : virtual public Clearable
{
    virtual void Register(Clearable* object) = 0;

    virtual void Unregister(Clearable* object) = 0;
};



struct Gate;



//  网表操作接口
struct NetListTable : virtual public Interface
{
    virtual int Load(Reader& reader) = 0;

    virtual int Save(Writer& writer) = 0;

    virtual const Gate* GateOf(const std::string& name) const = 0;

    virtual Gate* GateOf(const std::string& name) = 0;

    virtual const Gate* GateOf(GateId gateId) const = 0;

    virtual Gate* GateOf(GateId gateId) = 0;

    virtual const std::vector<const Gate*>& GetGates() const = 0;

    virtual std::vector<Gate*>& GetGates() = 0;
};



//
struct FaultListTable : virtual public Clearable
{
    virtual int Create(NetListTable& netlist) = 0;

    virtual int Load(Reader& reader) = 0;

    virtual int Save(Writer& writer) = 0;

    virtual void UpdateFaultStatus(FaultStatus status) = 0;

    virtual const vector<const Fault*> GetFaults() const = 0;

    virtual vector<Fault*> GetFaults() = 0;
};



struct Cube;



//! Cube 输出接口
struct CubeOutput : virtual public Interface
{
    virtual int Handle(Cube* cube) = 0;
};



//! Pattern 输出接口
struct PatternOutput : virtual public Interface
{
    virtual int Handle(Pattern* pattern) = 0;
};



struct FaultMaker : virtual public Interface
{
    virtual void UpdateFaultStatus(int32_t faultIndex, FaultStatus status) = 0;
};



struct ATPGDriver : virtual public Clearable
{
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, Options* options) = 0;

    //! 设置输出
    virtual int Setup(CubeOutput* output) = 0;

    //! 执行探测
    virtual int Detect(Fault* fault) = 0;
};



struct SimulateDriver : virtual public Clearable
{
    //! 设置输入
    virtual int Setup(int id, const NetListTable* netlist, const FaultListTable* faultlist, Options* options) = 0;

    //! 设置输出
    virtual int Setup(CubeOutput* output, FaultMaker* marker) = 0;

    //! 执行仿真
    virtual int Simulate(Pattern* pattern) = 0;
};



#endif  // HIATPG_H
