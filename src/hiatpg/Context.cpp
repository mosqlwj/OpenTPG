//
// Created by luolijun on 2020/7/6.
//

#include "Context.h"

#include <cassert>


#if defined(DEBUG)
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr)
#endif

template<typename T>
static inline void module_insert_tail(T* item, T* prev, T* next) {
    item->next = next;
    item->prev = prev;
    next->prev = item;
    prev->next = item;
}

struct ModuleImpl {
    ModuleDefinition defines;
    Module* object;
    ModuleImpl() { object = nullptr; }
};

struct Context {
    ModuleImpl modules[MODULE_COUNT_MAX];
};

Context context;

extern int DefineModule(int mid, ModuleDefinition* def) {
    ASSERT(mid >= 0);
    ASSERT(mid < MODULE_COUNT_MAX);
    ASSERT(def != nullptr);
    ASSERT(nullptr != def->creator);
    ASSERT('\0' != def->name[0]);
    ModuleImpl* impl = context.modules + mid;
    module_insert_tail(def, &impl->defines, impl->defines.prev);
    return 0;
}

extern int CreateModule(int mid, const char* name) {
    ASSERT(mid >= 0);
    ASSERT(mid < MODULE_COUNT_MAX);
    ASSERT(name != nullptr);

    ModuleImpl* impl = context.modules + mid;
    ModuleDefinition* def = nullptr;
    for (auto item = impl->defines.next; item != &(impl->defines); item = item->next) {
        if (0 == strcmp(name, item->name)) {
            def = item;
            break;
        }
    }

    if (nullptr == def) {
        return -1;
    }

    Module* module = def->creator();
    if (nullptr == module) {
        return -1;
    }

    impl->object = module;
    return 0;
}

extern Module* ImportModule(int mid) {
    ASSERT(mid >= 0);
    ASSERT(mid < MODULE_COUNT_MAX);

    ModuleImpl* impl = context.modules + mid;
    if (nullptr == impl->object) {
        if (&(impl->defines) == impl->defines.next) {
            return nullptr;
        }
        Module* module = impl->defines.next->creator();
        if (nullptr == module) {
            return nullptr;
        }

        impl->object = module;
    }

    return impl->object;
}
