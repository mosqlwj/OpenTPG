//
// Created by luolijun on 2020/7/6.
//

#ifndef HIATPG_CONTEXT_HPP
#define HIATPG_CONTEXT_HPP


#include "model.h"

struct Options;


enum {
    MODULE_COUNT_MAX = 10,
    MODULE_NAME_LEN_MAX = 11,
};


struct Module;
typedef Module* (*ModuleCreator)();
struct ModuleDefinition {
    ModuleDefinition* next;
    ModuleDefinition* prev;
    char name[MODULE_NAME_LEN_MAX + 1];
    ModuleCreator creator;
    ModuleDefinition() {
        next = this;
        prev = this;
        name[0] = '\0';
        creator = nullptr;
    }
};

extern int DefineModule(int mid, ModuleDefinition* def);
extern int CreateModule(int mid, const char* name);
extern Module* ImportModule(int mid);

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


struct Module {
    virtual ~Module() {}
    virtual int Init(Options* options) = 0;
    virtual void Clear() = 0;
    virtual void Destroy() = 0;
};


#endif  // HIATPG_CONTEXT_HPP
