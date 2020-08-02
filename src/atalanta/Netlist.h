// Netlist.h: interface for the Netlist class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ATALANTA_GATENET_H__
#define __ATALANTA_GATENET_H__

#include "Stack.h"
#include "Hash.h"
#include "Parameters.h"
#include "Globals.h"

namespace hiatpg {
	class Netlist
	{
	protected:
		unsigned int numberOfGates;
		unsigned int numberOfFlipFlops;
		unsigned int numberOfPrimaryInputs;
		unsigned int numberOfPrimaryOutputs;
		int PPOlevel;
		int POlevel;

		vector<int> primaryIn;
		vector<int> headlines;
		vector<int> primaryOut;
		int *flipFlops;

		vector<Gate*> gates; //Gate **gates;

		Stack *freeGates;
        Stack *faultyGates;
        Stack *evalGates;
        Stack *activeStems;
		Stack *dynamicStack;
		Stack *stack;
		Stack *stack1,*stack2;
		int nsStack,ndStack;

		Hash hashTable;

	public:
		Netlist(): hashTable(HASHSIZE), stack(0)
		{
			numberOfGates=numberOfFlipFlops=numberOfPrimaryInputs=numberOfPrimaryOutputs=0;
			PPOlevel=POlevel=0;
			flipFlops = nullptr;
			primaryOut.clear();//primaryOut=0;
			headlines.clear();  //headlines=0;
			primaryIn.clear(); //primaryIn = 0;
			gates.clear(); //gates=0;
			faultyGates=evalGates=activeStems=0;
			dynamicStack=stack=stack1=stack2=0;
			nsStack=ndStack=0;
		}

		void setTestAbility();
		void allocateDynamicBuffers();
		void allocateStacks();
		void checkParameters();
	};
}
#endif // __ATALANTA_GATENET_H__
