// EventList.h: interface for the EventList class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ATALANTA_STACK_H__
#define __ATALANTA_STACK_H__

#include "Gate.h"
#include <vector>

namespace hiatpg {
    class Stack
    {
    private:
        vector<Gate*>stackData;

    public:
        Stack(int maxCount) {stackData.reserve(maxCount);};
        void push(Gate *gate) { stackData.push_back(gate);};
        Gate* pop() {Gate *g = stackData.back();
            stackData.pop_back();
            return g;
        };
        void clear() {stackData.clear();};
        bool isEmpty() {return stackData.empty();};
        void deleteItem(int i) {stackData[i]=stackData.back(); stackData.pop_back();};
        int getCount() {return stackData.size();};
        void setCount(int i) {stackData.resize(i);};
        void copyFrom(Stack *src) {
            this->clear();
            stackData=src->stackData;
        }
        Gate*&  operator[](const int index) {return stackData[index];};
        void resetFreach() {for(unsigned int i=0;i<stackData.size();i++) stackData[i]->freach=false;};
    };
}
#endif //__ATALANTA_STACK_H__
