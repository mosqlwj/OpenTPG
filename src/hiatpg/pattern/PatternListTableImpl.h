//
// Created by luolijun on 2020/7/16.
//

#ifndef OPENTPG_PATTERNLISTTABLEIMPL_H
#define OPENTPG_PATTERNLISTTABLEIMPL_H

#include "HIATPG.h"

class PatternListTableImpl : public PatternListTable
{
public:
    //! 枚举所有的 Pattern
    virtual void Access(std::function<int(Pattern*)> handler) override ;

    //! 枚举所有的 Pattern
    virtual void Access(std::function<int(const Pattern*)> handler) override;

    virtual void Clear() override ;
};



#endif //OPENTPG_PATTERNLISTTABLEIMPL_H
