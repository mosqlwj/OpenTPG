// Fault.cpp: implementation of the Fault class.
//
//////////////////////////////////////////////////////////////////////
#include <list>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Error.h"

#include "Defines.h"
#include "Truthtable.h"
#include "Parameters.h"

#include "Hash.h"

#include "Gate.h"
#include "Stack.h"
#include "Netlist.h"
#include "FanNet.h"
#include "ParralelPattern.h"

#include "Fault.h"


#include "Globals.h"

namespace hiatpg {

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	Fault::Fault()
	{

	}

	void Fault::printFault(fstream *fp,bool mode, char cctMode)
	{
		Gate* gut;

#ifdef ISCAS85_NETLIST_MODE
		if(cctMode==ISCAS89)
		{
			if(line >= 0)
			{
				gut = gate->inlis[line];
				*fp<<gut->symbol->symbol.c_str()<<"->";
			}

			*fp<<gate->symbol->symbol.c_str();
			*fp<<fault2str[type];
		} else
		{
			if(line<0)
				*fp<<"Output line s-a-"<<type;
			else
				*fp<<"Input line "<<(line+1)<<"s-a-"<<type;
			*fp<<"of gate "<<gate->gid;
		}
#else
		if(line >= 0)
		{
			gut = gate->fanins[line];
			*fp<<gut->symbol->symbol.c_str()<<"->";
		}
		*fp<<gate->symbol->symbol.c_str();
		*fp<<" ";
		*fp<<fault2str[type];
#endif
		if(mode)
			switch(detected)
		{
			case DETECTED: *fp<<" detected"; break;
			case UNDETECTED: *fp<<" undetected"; break;
			case PROCESSED: *fp<<" aborted"; break;
			case REDUNDANT: *fp<<" redundant";
		}
		*fp<<endl;
	}

	Fault::~Fault()
	{

	}

	//	addFault
	//	Add a fault to the linked list of each gate.

	void Fault::addFault()
	{
		gate->pfault.push_back(this);
	}

	//	restore_detected_fault_list
	//	Restores the fault list for test compaction.
	//	Does not restore redundant faults.

	int FaultList::restoreDetectedFaultList()
	{
		Fault *f;
		int n=0;

		for(int i=0;i<numberOfFaults;i++)
		{
			f=faultList[i];
			if(f->detected==DETECTED)
			{
				f->detected=UNDETECTED;
				f->addFault();
				n++;
			}
		}

		return n;
	}

	int FaultList::checkRedundantFaults()
	{
		Gate *gut;
		Fault *f;

		int n=0, j;

		for(int i=0;i<numberOfGates;i++)
			if(gates[i]->noutput > 1)
			{
				gut=gates[i];
				for(j=0;j<gut->noutput;j++) gut->fanouts[j]->changed++;
				for(j=0;j<gut->noutput;j++)
				{
					if(gut->fanouts[j]->changed > 1)
					{
						list<Fault*>::iterator current,final;

						current=gut->fanouts[j]->pfault.begin();
						final=gut->fanouts[j]->pfault.end();

						while(current!=final)
						{
							f=*current;
							if(f->line>=0)
							{
								if(f->gate->fanins[f->line] == gut)
								{
									f->detected=REDUNDANT;
									current=f->gate->pfault.erase(current);		
									//current--;
									n++;
								} else current++;
							} else current++;
						}
					}
					gut->fanouts[j]->changed=0;
				}
			}
			return n;
	}

	int FaultList::createFaultList(int noStem, Gate **stem)
	{
		Gate *gate;
		Fault* fault;

		Fault *current;
		Fault *curr;
		FaultType faultType;
		int nfault,n,nof,i;
		int *test,size;

		test=new int;
		size=sizeof(Fault);
		nfault=0;
		curr=new Fault;
		current=new Fault;

		// create fault for each gate.
		for(i=0;i<numberOfGates;i++)
		{
            gate=gates[i];
			/* if the input of the gate has more than one fanouts, 
			add a s-a-1 for each AND/NAND,
			a s-a-0 for each OR/NOR and
			a s-a-0 and s-a-1 for other gates. */
			if(gate->ninput > 1)
			{
                faultType= (gate->type == AND || gate->type == NAND) ? SA1 : SA0;
				for(int j=0; j < gate->ninput; j++)
				{
					if(gate->fanins[j]->noutput > 1)
					{
                        fault=new Fault;
                        fault->gate=gate;
                        fault->type=faultType;
                        fault->line=j;
						nfault++;
						gate->pfault.push_back(fault);

						/* case of high level gates */
						if(gate->type > PI)
						{
                            fault=new Fault();
                            fault->gate=gate;
                            fault->type= (faultType == SA1) ? SA0 : SA1;
                            fault->line=j;
							nfault++;

							gate->pfault.push_back(fault);
						}
					}
				}
			}
			if((gate->noutput == 1) &&
               (gate->fanouts[0]->ninput > 1 || gate->fanouts[0]->type == PO))
			{
                faultType= (gate->fanouts[0]->type == OR || gate->fanouts[0]->type == NOR) ? SA0 : SA1;
                fault=new Fault;
                fault->gate=gate;
                fault->type=faultType;
                fault->line=OUTFAULT;
				nfault++;
				gate->pfault.push_back(fault);

				// case of high level gates 
				if(gate->fanouts[0]->type > PI)
				{
                    fault=new Fault;
                    fault->gate=gate;
                    fault->type= (faultType == SA1) ? SA0 : SA1;
                    fault->line=OUTFAULT;
					nfault++;
					gate->pfault.push_back(fault);
				}
			} else if(gate->noutput > 1) {
                fault=new Fault();
                fault->gate=gate;
                fault->type=SA1;
                fault->line=OUTFAULT;
				nfault++;
				gate->pfault.push_back(fault);

                fault=new Fault();
                fault->gate=gate;
                fault->type=SA0;
                fault->line=OUTFAULT;
				nfault++;
				gate->pfault.push_back(fault);
			} else if(gate->type == PO && gate->fanins[0]->noutput > 1) {
                fault=new Fault();
                fault->gate=gate;
                fault->type=SA1;
                fault->line=0;
				nfault++;
				gate->pfault.push_back(fault);

                fault=new Fault();
                fault->gate=gate;
                fault->type=SA0;
                fault->line=0;
				nfault++;
				gate->pfault.push_back(fault);
			}
		}

		// create the fault_list and
		// enumerate faults in each fanout free region
		faultList=new Fault*[nfault];
		stack->clear();

		nof=0;
		for(i=noStem-1;i>=0;i--)
		{
			stack->push(stem[i]);
			n=1;
			while(!stack->isEmpty())
			{
                gate=stack->pop();

				list<Fault*>::iterator current,final;

				current=gate->pfault.begin();
				final=gate->pfault.end();

				while(current!=final)
				{
					faultList[nof++]=*current;
					current++;
					n++;
				}
				for(int j=0; j < gate->ninput; j++)
					if(gate->fanins[j]->noutput == 1) stack->push(gate->fanins[j]);
			}
			stem[i]->dfault=new Fault*[n];
		}

		if(nfault==nof) return nfault; else return -1;
	}

#ifdef INCLUDE_HOPE

	void FaultList::setParity(Gate *gut,int par) {gut->changed=inverseParity[parityOfGate[gut->type]][par];}
	void FaultList::mark(Gate *gut) {gut->changed+=2;}
	bool FaultList::isStem(Gate *gut) {return ((gut->noutput != 1) || (gut->fanouts[0]->type == DFF));}
	bool FaultList::isNotMarked(Gate *gut) {return gut->changed<2;}

	void FaultList::insertFault(Gate *gut,int line,FaultType type)
	{
		int parity;
		Fault *f;

		parity = (gut->changed>=2) ? gut->changed-2 : gut->changed;
		if(line<0) parity = inverseParity[parityOfGate[gut->type]][parity];

		f=new Fault();
		f->gate=gut;
		f->line=line;
		f->type=type;
		f->npot=0;

		numberOfFaults++;

		if((parity==0 && type==SA0) || (parity==1 && type==SA1))
			evenList->push_back(f);
		else
			oddList->push_back(f);
	}

	void FaultList::defaultLineFault(Gate *gut,int line)
	{
		Gate *from,*to;

		if(line<0)
		{
			//output line fault
			if(gut->type == DUMMY || gut->type == PO) return;
			if(gut->noutput!=1)
			{
				insertFault(gut,OUTFAULT,SA0);
				insertFault(gut,OUTFAULT,SA1);
			} else
			{
				to=gut->fanouts[0];
				if(to->type == DUMMY) to=to->fanouts[0];
				switch(to->type)
				{
                    case AND:
                    case NAND:
                        if(to->ninput>1) insertFault(gut,OUTFAULT,SA1); break;
                    case OR:
                    case NOR:
                        if(to->ninput>1) insertFault(gut,OUTFAULT,SA0); break;
                    case XOR:
                    case XNOR:
                    case DFF:
                    case PO:
                        insertFault(gut,OUTFAULT,SA0);
                        insertFault(gut,OUTFAULT,SA1);
                        break;
				    default:
				        break;
				}
			}
		} else 
		{
			from=gut->fanins[line];
			if(from->type == DUMMY || from->type == PO) from=from->fanins[0];
			if(from->noutput>1)
				switch(gut->type)
                {
                    case AND:
                    case NAND:
                        if(gut->ninput>1) insertFault(gut,line,SA1); break;
                    case OR:
                    case NOR:
                        if(gut->ninput>1) insertFault(gut,line,SA0); break;
                    case XOR:
                    case XNOR:
                    case DFF:
                    case PO:
                        insertFault(gut,line,SA0);
                        insertFault(gut,line,SA1);
                        break;
                    default:
                        break;
                }
		}
	}

	void FaultList::FFRfault(Gate *gut)
	{
		Gate* temp;
		oddList=new list<Fault*>;
		evenList=new list<Fault*>;

		stack1->clear();
		stack1->push(gut);

		while(!stack1->isEmpty())
		{
			gut=stack1->pop();
			defaultLineFault(gut,OUTFAULT);
			for(int ix=0;ix<gut->ninput;ix++)
			{
				temp=gut->fanins[ix];
				if(isStem(temp))
					defaultLineFault(gut,ix);
				else
					stack1->push(temp);
			}
		}

		if(evenList->size() > 0) {
		  hopeFaultList.insert(hopeFaultList.end(),evenList->begin(),evenList->end());
		}
		if(oddList->size() > 0) {
			hopeFaultList.insert(hopeFaultList.end(),oddList->begin(),oddList->end());
		}

		delete oddList;
		delete evenList;
	}

	void FaultList::DFSpo(Gate *parent,Gate *child)
	{
		//preWORK
		setParity(child,(parent==0 ? 0 : parent->changed-2));
		mark(child);

		if(isStem(child)) FFRfault(child);

		// Go into children 
		for(int i=0;i<child->ninput;i++)
		{
			// preWORK for input lines
			if(isNotMarked(child->fanins[i]))
				DFSpo(child,child->fanins[i]);
		}
	}

	void FaultList::FWDfaults()
	{
		Gate *gut;
		int i;

		for(i=0;i<numberOfGates;i++) gates[i]->changed=0;

		//   init_fault_list();

		// Primary Outputs
		for(i=0;i<numberOfPrimaryOutputs;i++)
		{
			gut=gates[primaryOut[i]];
			DFSpo(0,gut);
		}

		// count faults and copy
		numberOfFaults=hopeFaultList.size();

		faultList=new Fault*[numberOfFaults];

		list<Fault*>::iterator current,final;

		current=hopeFaultList.begin();
		final=hopeFaultList.end();

		i=0;
		while(current!=final)
		{
			faultList[i]=*current;
			i++;
			current++;
		}
	}

	int FaultList::restoreHopeFaultList()
	{
		Fault *f;
		int i,n;

		n=0;

		hopeFaultList.clear();

		for(i=0;i<numberOfFaults;i++)
		{
			f=faultList[i];
			if(f->detected==DETECTED)
			{
				f->detected=UNDETECTED;
				hopeFaultList.push_back(f);
				n++;
			}
			if(!f->event.empty())
			{
				hopeEventList.insert(hopeEventList.begin(),f->event.begin(),f->event.end());
				f->event.clear();
			}
		}
		return n;
	}

#endif

	char ReadableFaultList::getFaultSymbol(string *s)
	{
		char c = 0;
		int n=0;
		status valid=false;

		s->clear();
		while(!inputf->eof())
		{
			c = inputf->get();
			if(c == -1) continue;
			
			if(isWhitespace(c)) {if(valid) break; else continue;}
			if(isHeadSymbol(c)) {s->append(1, c);continue;}
			if(isValid(c)) {s->append(1, c); valid=true;}
			else Error::fatalerror(FAULTERROR);
		};
		if(inputf->eof()) {
			c=EOF;
		}
		//s[n]=EOS;
		return c;
	}

	void ReadableFaultList::readFaults(std::streambuf *fn)
	{
		if (fn != NULL)
		{
			istream fault(fn);

			numberOfFaults = readFaultsFsim(&fault,myNumberOfStems,myStem);

			if(numberOfFaults<0)
			{
				/*cerr<<"Fatal error: error in setting fault list"<<endl;
				exit(0);*/
				stringstream ss;
				ss << "Fatal error: error in setting fault list";
				throw ss.str();
			}
		}
	}

	int ReadableFaultList::readFaultsFsim(istream *file,int noStem,Gate **stem)
	{
		inputf=file;

		Gate *gut;
		Fault *f;
		HashData *h;
		int from,to,line,type;
		string s; //char s[MAXSTRING];
		int nfault,n,nof;

		nfault=0;

		while(getFaultSymbol(&s)!=EOF)
		{
			if(isValid(s[0]))
			{
				if((h=hashTable.findHash(s,0)) ==0)
				{
					cout<<"Error in fault file:";
					cout<<s;
					cout<<" is not defined\n";
					Error::fatalerror(FAULTERROR);
				}
				if((to=h->pnode->index) < 0) Error::fatalerror(FAULTERROR);
				gut=gates[to];
				line=-1;
			} else if(s[0]=='>')
			{
				from=to;
				if((h=hashTable.findHash(string(&s[1]),0)) ==0)
				{
					cout<<"Error in fault file:";
					cout<<s;
					cout<<" is not defined\n";
					Error::fatalerror(FAULTERROR);
				}
				if((to=h->pnode->index)<0) Error::fatalerror(FAULTERROR);
				gut=gates[to];
				for(int i=0;i<gut->ninput;i++)
					if(gut->fanins[i]->index == from) { line=i; break;};
			} else if(s[0]=='/')
			{
				if(s[1]=='1') type=SA1; else type=SA0;
				if(line>=0)
					type=(type==SA1) ? SA1 : SA0;

				f=new Fault;
				f->gate=gut;
				f->line=line;
				f->type= static_cast<FaultType>(type);
				gut->pfault.push_front(f);
				nfault++;
			} else Error::fatalerror(FAULTERROR);
		}

		// create the fault_list and
		// enumerate faults in each fanout free region
		faultList=new Fault*[nfault];
		stack->clear();

		nof=0;
		for(int i=noStem-1;i>=0;i--)
		{
			stack->push(stem[i]);
			n=1;
			while(!stack->isEmpty())
			{
				gut=stack->pop();

				list<Fault*>::iterator current,final;

				current=gut->pfault.begin();
				final=gut->pfault.end();

				while(current!=final)
				{
					faultList[nof++]=*current;
					n++;
					current++;
				}
				for(int j=0;j<gut->ninput;j++)
					if(gut->fanins[j]->noutput == 1) stack->push(gut->fanins[j]);
			}
			stem[i]->dfault=new Fault*[n];
		}	

		if(nfault==nof) return(nfault);

		return -1;
	}

#ifdef INCLUDE_HOPE
	void ReadableFaultList::readFaultsHope(istream *file)
	{
		Gate*	gut;
		Fault*	f;
		int from,to,line,type;
		HashData*	h;
		string s; //char s[MAXSTRING];
		int i;

		inputf=file;

		//init_fault_list();
		while(getFaultSymbol(&s)!=EOF)
		{
			if(isValid(s[0]))
			{
				if((h=hashTable.findHash(s,0))==0)
				{
					cout<<"Error in fault file:";
					cout<<s;
					cout<<" is not defined\n";
					Error::fatalerror(FAULTERROR);
				}
				if((to=h->pnode->index)<0) Error::fatalerror(FAULTERROR);
				gut=gates[to];
				line=OUTFAULT;
			} else if(s[0]=='>')
			{
				from=to;
				if((h=hashTable.insertHash(string(&s[1]),0))==0)
				{
					cout<<"Error in fault file:";
					cout<<s;
					cout<<" is not defined\n";
					Error::fatalerror(FAULTERROR);
				}

				if((to=h->pnode->index)<0) Error::fatalerror(FAULTERROR);
				gut=gates[to];
				for(i=0;i<gut->ninput;i++)
					if(gut->fanins[i]->index == from) { line=i; break; }
			} else if(s[0]=='/')
			{
				if(s[1]=='1') type=SA1; else type=SA0;
				f=new Fault;
				f->gate=gut;
				f->line=line;
				f->type=static_cast<FaultType>(type);

				hopeFaultList.push_front(f);
			} else Error::fatalerror(FAULTERROR);
		}

		// count faults and copy 
		numberOfFaults=hopeFaultList.size();

		faultList=new Fault*[numberOfFaults];

		list<Fault*>::iterator current,final;

		current=hopeFaultList.begin();
		final=hopeFaultList.end();

		i=0;
		while(current!=final)
		{
			faultList[i]=*current;
			i++;
			current++;
		}

	}
#endif
}