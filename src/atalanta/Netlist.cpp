// Netlist.cpp: implementation of the Netlist class.
//
//////////////////////////////////////////////////////////////////////
#include "Netlist.h"

#include <string.h>

#include <iostream>
#include <list>
#include <sstream>

#include "AtpgEngine.h"
#include "Defines.h"
#include "FanNet.h"
#include "Gate.h"
#include "Globals.h"
#include "Stack.h"

namespace hiatpg {
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
	
	void Netlist::checkParameters()
	{
		if(numberOfGates<=0 || numberOfPrimaryInputs<=0 || numberOfPrimaryOutputs<=0)
		{
			stringstream ss;
			ss <<"Error: #pi="<<numberOfPrimaryInputs<<", #po="<<numberOfPrimaryOutputs<<", #gate="<<numberOfGates;
			throw ss.str();
		}
		
		if(numberOfFlipFlops > 0)
		{
			stringstream ss;
			ss  << "Error: "<<numberOfFlipFlops<<" flip-flop exist in the circuit";
			throw ss.str();
		}
	}
	
	level Netlist::logiclevel(level V0, level V1, int n)
	{
		V0=((V0 & BITMASK[n]) == ALL0) ? ZERO : ONE;
		V1=((V1 & BITMASK[n]) == ALL0) ? ZERO : ONE;
		return(parallelToLevel[V0][V1]);
	}
	
	void Netlist::allocateStacks()
	{
		stack1=new Stack(numberOfGates+numberOfPrimaryOutputs);
		stack2=new Stack(numberOfGates+numberOfPrimaryOutputs);
	}
	
	string* Netlist::printInputs(int nth_bit)
	{
		string *s=new string;
		s->resize(numberOfPrimaryInputs, '0');
		
		for(int j=0;j<numberOfPrimaryInputs;j++)
			if(checkBit(gates[j]->output1, nth_bit))
				(*s)[j] = '1'; 
			
			return s;
	}
	
	string* Netlist::printOutputs(int nth_bit)
	{
		string * s=new string;
		s->resize(numberOfPrimaryOutputs, '0');
		
		for(int j=0;j<numberOfPrimaryOutputs;j++)
			if(checkBit(gates[primaryOut[j]]->output1, nth_bit))
				(*s)[j] = '1';
			return s;
	}

    void Netlist::setTestAbility()
	{
		int i,j,depth;
		
		// cont0 and cont1
		for(i=0;i<numberOfGates;i++) 
		{
			if(gates[i]->isFree() || gates[i]->isHead()) gates[i]->cont0=0;
			else
			{
				if(i==200)
				{
					i=i;
				}
				
				
				depth=-1;
				for(j=0; j < gates[i]->ninput; j++)
					depth=MAX(depth, gates[i]->fanins[j]->cont0);
                gates[i]->cont0= depth + 1;
			}
            gates[i]->cont1=gates[i]->cont0;
		}
		
		/* depth from output */
		for(i=numberOfGates-1;i>=0;i--)
		{
			if(gates[i]->type == PO)
                gates[i]->dpo=0;
			else
			{
				depth=-1;
				for(j=0; j < gates[i]->noutput; j++)
					depth=MAX(depth, gates[i]->fanouts[j]->dpo);
                gates[i]->dpo= depth + 1;
			}
		}
	}
	
	void Netlist::allocateDynamicBuffers()
	{
		freeGates=new Stack(numberOfGates);
		faultyGates=new Stack(numberOfGates);
		evalGates=new Stack(numberOfGates);
		activeStems=new Stack(numberOfGates);
		dynamicStack=new Stack(numberOfGates);
	}
	
}
