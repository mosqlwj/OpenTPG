//
// Created by luolijun on 2020/7/16.
//

#include "PatternListTableImpl.h"



IMPLEMENT_MODULE(ModuleID::MODULE_PATTERNLIST, PatternListTableImpl, "default");



void PatternListTableImpl::Access(std::function<int(Pattern*)> handler)
{
}


//! 枚举所有的 Pattern
void PatternListTableImpl::Access(std::function<int(const Pattern*)> handler)
{
}

void PatternListTableImpl::Clear()
{
}
