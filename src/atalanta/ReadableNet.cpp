// ReadableNet.cpp: implementation of the ReadableNet class.
//
//////////////////////////////////////////////////////////////////////
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

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

#include "FaultSimulation.h"
#include "ReadableNet.h"
#include "Params.h"

namespace hiatpg {
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	//ReadableNet::ReadableNet():hashTable(HASHSIZE),maxFout(0)
int ReadableNet::parseNetlist(const string& netlistFileName) {
		int i, j;

		if(readCircuit(netlistFileName) <0) {
			Error::fatalerror(CIRCUITERROR);
		}

		checkParameters();
		addPO();

        stack=new Stack(numberOfGates+10);

        allocateStacks();
        maxlevel=computeLevel();
        allocateEventList();
        levelize();
        addSpareGate();

        i=setFFR();
        j=setDominator();

        if(numberOfFlipFlops > 0)
        {
            stringstream ss;
            ss << "Error: Invalid type DFF is defined.";
            throw ss.str();
        }

		setCctParameters();

#ifdef INCLUDE_HOPE
		levels=maxlevel+2;
#else
		levels=maxlevel;
#endif
        allocateDynamicBuffers();

        myNumberOfStems=0;
        for(i=0;i<numberOfGates;i++)
            if(gates[i]->isFanout() || gates[i]->type == PO) myNumberOfStems++;
        myStem=new Gate*[myNumberOfStems];
        setFanoutStemp(myStem,myNumberOfStems);
		return levels;
	}

	/*int ReadableNet::readBenchFile(string benchFile)
	{
		fstream f;
		int lev;

		if (!benchFile.empty())
		{
			f.open(benchFile.c_str(),ios::in);
			if(!f.is_open())
			{
				cerr<<"Fatal error: no such file exists "<<benchFile<<endl;
				exit(0);
			}
			lev = readBenchStream(f.rdbuf());
			f.close();
			return lev;

		}

		return levels;
	}*/

	int ReadableNet::gateType(string *symbol)
	{
		int fn;

		if(symbol->compare("NOT") == 0) fn=NOT;
		else if(symbol->compare("AND") == 0) fn=AND;
		else if(symbol->compare("NAND") == 0) fn=NAND;
		else if(symbol->compare("OR")== 0) fn=OR;
		else if(symbol->compare("NOR") == 0) fn=NOR;
		else if(symbol->compare("DFF") == 0) fn=DFF;
		else if(symbol->compare("XOR") == 0) fn=XOR;
		else if(symbol->compare("XNOR") == 0) fn=XNOR;
		else if(symbol->compare("BUFF") == 0) fn=BUFF;
		else if(symbol->compare("BUF") == 0) fn=BUFF;
		else if(symbol->compare("INPUT") == 0) fn=PI;
		else if(symbol->compare("OUTPUT") == 0) fn=PO;
		else if(symbol->compare("not") == 0) fn=NOT;
		else if(symbol->compare("and") == 0) fn=AND;
		else if(symbol->compare("nand") == 0) fn=NAND;
		else if(symbol->compare("or")== 0) fn=OR;
		else if(symbol->compare("nor") == 0) fn=NOR;
		else if(symbol->compare("dff") == 0) fn=DFF;
		else if(symbol->compare("xor") == 0) fn=XOR;
		else if(symbol->compare("xnor") == 0) fn=XNOR;
		else if(symbol->compare("buff") == 0) fn=BUFF;
		else if(symbol->compare("buf") == 0) fn=BUFF;
		else if(symbol->compare("input") == 0) fn=PI;
		else if(symbol->compare("output") == 0) fn=PO;
		else fn=(-1);

		return(fn);
	}

	char ReadableNet::getSymbol(string *s)
	{
		char c;
		int comm=0;

		s->clear();
		while(!circuit.eof())
		{
			circuit>>c;

			if(c=='#')
			{
				circuit.ignore(INT32_MAX,'\n');
				continue;
			}
			/*		if(comm==1)
			{
			if(c=='\n') comm=0;
			continue;
			}*/
			if(isWhiteSpace(c)) continue;
			if(isDelimiter(c)) break;
			s->append(1, c);//*(s++)=c;
		};
		if(circuit.eof()) c=EOF;
		//*s=EOS;
		return c;
	}

    int ReadableNet::readCircuit(const string &netlistFileName) {
        fstream netlistStream;
        netlistStream.open(netlistFileName, ios::in);
        circuit.rdbuf(netlistStream.rdbuf());

		char c=0;
		int i, j;

		int numberOfGates=0;
		int numberOfFlipFlops=0;
		int numberOfPrimaryInputs=0;
		int numberOfPrimaryOutputs=0;

		string symbol; //char symbol[MAXSTRING];
		int nofanin=0;
		int fn;
		int netSize;
		int nerrs=0;

		HashData *hashPointer;
		Gate *currentGate;
		Gate *pg;

		vector<Gate *> poGates; //Gate	 *poGates[MAXPO+100];
		vector<Gate *> pfanin; //Gate	 *pfanin[MAXFIN+100];

		Gate *begnet=0;
		symbol.reserve(200);
		this->numberOfPrimaryInputs = this->numberOfPrimaryOutputs =
			this->numberOfGates = this->numberOfFlipFlops = 0;
		gates.clear();
		testVector.vectors.clear();
		hashTable.clear();

		/* Pass 1:
		Adds the gate symbols to symbol_tbl[] and
		counts # of gates, pi's, po's, ff's */

		while((c=getSymbol(&symbol)) != EOF )
		{
			switch(c)
			{
			case '=' :	//a new gate
				hashPointer=hashTable.findAndInsertHash(symbol,0);
				if((currentGate=hashPointer->pnode) == 0)
				{
					currentGate=new Gate();
					hashPointer->pnode=currentGate;
					currentGate->symbol=hashPointer;
					currentGate->next=begnet;
					begnet=currentGate;
				}
				break;
			case '(' :	//gate type
				if((fn=gateType(&symbol)) < 0 )
				{
					fprintf(stderr,"Error: Gate type %s is not valid\n",symbol.c_str());
					return -1;
				}
				break;
			case ',' :	//fanin list
				hashPointer=hashTable.findAndInsertHash(symbol,0);
				if((pg=hashPointer->pnode) == 0)
				{
					pg=new Gate();
					hashPointer->pnode=pg;
					pg->symbol=hashPointer;
					pg->index=-1;
					pg->next=begnet;
					begnet=pg;
				}
				if(nofanin >= pfanin.size()) {
					pfanin.resize(nofanin + 1, NULL);
				}
				pfanin[nofanin++]=pg;
				break;
			case ')' : //terminator, fanin list
				hashPointer=hashTable.findAndInsertHash(symbol,0);
				if((pg=hashPointer->pnode) == 0)
				{
					pg=new Gate();
					hashPointer->pnode=pg;
					pg->symbol=hashPointer;
					pg->index=-1;
					pg->next=begnet;
					begnet=pg;
				}
				switch(fn)
				{
				case PI:
					numberOfPrimaryInputs++;
					pg->index=numberOfGates++;
					pg->ninput=0;
					pg->fanins=0;
					pg->type=PI;
					pg->noutput=0;
					pg->fanouts=0;
					break;
				case PO:
					if(numberOfPrimaryOutputs >= poGates.size()) {
						poGates.resize(numberOfPrimaryOutputs + 1, NULL);
					}
					poGates[numberOfPrimaryOutputs++]=pg;
					break;
				default:
					if(nofanin >= pfanin.size()) {
						pfanin.resize(nofanin + 1, NULL);
					}
					pfanin[nofanin++]=pg;
					switch(fn)
					{
					case DFF:
						numberOfFlipFlops++;
						putchar('F');
						break;
					case XOR:
					case XNOR:
						if(nofanin!=2)
						{
							fprintf(stderr,"Error: %d-input %s gate is not supported\n",nofanin, fn_to_string[fn]);
							return -1;
						}
					}
					if(currentGate==0)
					{
						fprintf(stderr,"Error: Syntax error in the circuit file\n");
						return -1;
					}
					currentGate->index=numberOfGates++;
					currentGate->type=static_cast<GateType>(fn);
					if((currentGate->ninput=nofanin) == 0)
						currentGate->fanins=0;
					else
						currentGate->fanins=new Gate*[currentGate->ninput];
					for(i=0;i<nofanin;i++) {
						currentGate->fanins[i]=pfanin[i];
					}
					currentGate->noutput=0;
					currentGate->fanouts=0;

					nofanin=0;
					currentGate=0;
					break;
				}
			}
		}

		/*if(numberOfGates > MAXGATE) {
			//fprintf(stderr,"The number of gates exceeds MAXGATE %d\n",MAXGATE);
			//exit(0);
			stringstream ss;
			ss << "The number of gates exceeds MAXGATE " << MAXGATE;
			throw ss.str();
		}*/
		/*if(numberOfPrimaryInputs > MAXPI) {
			//fprintf(stderr,"The number of primary inputs exceeds MAXPI %d\n",MAXPI);
			//exit(0);
			stringstream ss;
			ss << "The number of primary inputs exceeds MAXPI " << MAXPI;
			throw ss.str();
		}*/
		/*if(numberOfPrimaryOutputs > MAXPO) {
			//fprintf(stderr,"The number of primary outputs exceeds MAXPO %d\n",MAXPO;
			//exit(0);
			stringstream ss;
			ss << "The number of primary outputs exceeds MAXPO " << MAXPO;
			throw ss.str();
		}*/

		// Pass 2: Construct the circuit data structure

		//netSize=numberOfGates+numberOfPrimaryOutputs+numberOfFlipFlops+SPAREGATES;

		//gates=new Gate*[netSize];
		//primaryIn=new int[numberOfPrimaryInputs];
		//primaryOut=new int[numberOfPrimaryOutputs];
		flipFlops=new int[numberOfFlipFlops+1];

#ifdef ATALANTA
		headlines.resize(numberOfPrimaryInputs);//headlines=new int[numberOfPrimaryInputs];
#endif
		// Check floating inputs
		for(currentGate=begnet; currentGate!=0; currentGate=currentGate->next)
		{
#ifdef _ALG_DEBUG
			dbgFile << currentGate->index << "\n";
			dbgFile.flush();
#endif
			if(currentGate->index<0)
			{
				fprintf(stderr,"Error: floating gates %s\n",currentGate->symbol->symbol.c_str());
				fprintf(stderr, "Workaround. You have to take one of the two actions:\n");
				fprintf(stderr, "   1. Remove all the floating input and associated gates, or\n");
				fprintf(stderr, "   2. Make each floating input a primary output.\n");
				return -1;
			}
			if(currentGate->index >= gates.size()) {
				gates.resize(currentGate->index + 1, NULL);
			}
            gates[currentGate->index]=currentGate;
			this->numberOfGates++;
		}

//		for(i=this->numberOfGates;i<netSize;i++) gates[i]=0;

		if(this->numberOfGates!=numberOfGates)
		{
			fprintf(stderr,"Error in read_circuit\n");
			return -1;
		}

		// Pass 3: Compute fanout list
		for(i=0;i<numberOfGates;i++)
		{
			currentGate=gates[i];
#ifdef LEARNFLG
			currentGate->pLearn.clear();
#endif
			for(int j=0;j<currentGate->ninput;j++) currentGate->fanins[j]->noutput++;
            switch (currentGate->type) {
                case PI:
                    primaryIn.push_back(i);
                    this->numberOfPrimaryInputs++;
                    break;
                case DFF:
                    flipFlops[this->numberOfFlipFlops++] = i;
                    break;
                default:
                    break;
            }
        }
		for(i=0;i<numberOfPrimaryOutputs;i++) {
			if(this->numberOfPrimaryOutputs >= primaryOut.size()) {
				primaryOut.resize(this->numberOfPrimaryOutputs + 1, 0);
			}
			primaryOut[this->numberOfPrimaryOutputs++]=poGates[i]->index;
		}
		for(i=0;i<this->numberOfGates;i++)
		{
			currentGate=gates[i];
			if(currentGate->noutput>0)
			{
				currentGate->fanouts=new Gate*[currentGate->noutput];
				maxFout=MAX(maxFout,currentGate->noutput);
				currentGate->noutput=0;
			}
		}

		for(i=0;i<this->numberOfGates;i++)
		{
			currentGate=gates[i];
			for(int j=0;j<currentGate->ninput;j++)
				currentGate->fanins[j]->fanouts[(currentGate->fanins[j]->noutput)++]=currentGate;
		};

		for(i=0; i<this->numberOfGates; i++)
		{
			currentGate=gates[i];
			if (currentGate->noutput > 0) continue;
			for(j=0;j<numberOfPrimaryOutputs;j++)
				if (currentGate == poGates[j]) break;
			if (j == numberOfPrimaryOutputs)
			{
				fprintf(stderr, "Error: floating output '%s' detected!\n", currentGate->symbol->symbol.c_str());
				nerrs++;
			}
		}

		if (nerrs > 0) {
			fprintf(stderr, "Workaround. You have to take one of the two actions:\n");
			fprintf(stderr, "   1. Remove all the floating output and associated gates, or\n");
			fprintf(stderr, "   2. Make each floating output a primary output.\n");
			return -1;
		}

		if(numberOfGates==this->numberOfGates)
			return this->numberOfGates;
		else
			return -1;
	}


#ifdef INCLUDE_HOPE
	int ReadableNet::computeLevel()
	{
		int i,j,flag=1;
		Gate *currentGate,*ng;

		for(i=0;i<numberOfGates;i++)
		{
			currentGate=gates[i];
			if(currentGate->type == PI || currentGate->type == DFF)
			{
				currentGate->dpi=0;
				stack1->push(gates[i]);
				currentGate->changed=currentGate->ninput;
			} else
			{
				currentGate->dpi=(-1);
				currentGate->changed=0;
			}
		}

		while(true)
		{
			if(flag==1)
				while(!stack1->isEmpty())
				{
					currentGate=stack1->pop();
					for(i=0;i<currentGate->noutput;i++)
					{
						ng=currentGate->fanouts[i];
						if(++ng->changed==ng->ninput)
						{
							ng->dpi=currentGate->dpi+1;
							stack2->push(ng);
						}
					}
				} else
					if(flag==2)
						while(!stack2->isEmpty())
						{
							currentGate=stack2->pop();
							for(i=0;i<currentGate->noutput;i++)
							{
								ng=currentGate->fanouts[i];
								if(++ng->changed==ng->ninput)
								{
									ng->dpi=currentGate->dpi+1;
									stack1->push(ng);
								}
							}
						}

						flag = (flag==1) ? 2 : 1;
						if(stack1->isEmpty() && stack2->isEmpty()) break; //exit
		}

		// Compute maxlevel
		maxlevel=-1;
		for(i=0;i<numberOfPrimaryOutputs;i++)
		{
			currentGate=gates[primaryOut[i]];
			if(currentGate->type == PO)
			{
				if(currentGate->dpi>maxlevel) {maxlevel=currentGate->dpi; flag=1;}
			}
			else
				if(currentGate->dpi>=maxlevel) {maxlevel=currentGate->dpi; flag=2; }
		}

		for(i=0;i<numberOfFlipFlops;i++)
		{
			currentGate=gates[flipFlops[i]];
			for(j=0; j<currentGate->ninput; j++)
				if(currentGate->fanins[j]->dpi >= maxlevel) { maxlevel=currentGate->fanins[j]->dpi; flag=2; }
		}

		// Renumber levels of POs and PPO(DFF)s
		if(flag==1) maxlevel--;
		POlevel=maxlevel+1;
		PPOlevel=maxlevel+2;
		for(i=0;i<numberOfPrimaryOutputs;i++)
			if(gates[primaryOut[i]]->type == PO) gates[primaryOut[i]]->dpi=POlevel;
		for(i=0;i<numberOfFlipFlops;i++) gates[flipFlops[i]]->dpi=PPOlevel;

		return(maxlevel+1);
	}

	int ReadableNet::levelize()
	{
		int i,newone=0;
		Gate* cg;

		// re-number gates
		for(i=0;i<numberOfGates;i++)
		{
			pushGate(gates[i]);
		}
		for(i=0;i<maxlevel+2;i++)
		{
			for(int j=0;j<=eventList[i]->getCount()-1;j++)
			{
				cg=(*eventList[i])[j];
				cg->index=newone++;
			}
			eventList[i]->clear();
		}

		// update gate numbers
		for(i=0;i<numberOfPrimaryInputs;i++)				// primaryin
			primaryIn[i]=gates[primaryIn[i]]->index;
		for(i=0;i<numberOfPrimaryOutputs;i++)				/* primaryout */
			primaryOut[i]=gates[primaryOut[i]]->index;
		for(i=0;i<numberOfFlipFlops;i++)				/* flip_flops */
			flipFlops[i]=gates[flipFlops[i]]->index;

		// sort gates by index
		i=0;
		while(i<numberOfGates)
		{
			if(i == gates[i]->index)
				i++;
			else 			/* swap gates[i] and gates[gates[i]->gid] */
			{
				cg=gates[i];
                gates[i]=gates[cg->index];
                gates[cg->index]=cg;
			}
		}
		return 0;
	}
#else

	int ReadableNet::levelize(int n,Gate **stack)
	{
		int i, numberOfGates;
		Gate **first,**last;
		Gate *ele,*next;
		int gIndex=0;
		int maxDpi=-1;

		// initialize
		for(i=0; i<n; i++)
			if((ele=gates[i]) != 0)
			{
				ele->changed=ele->ninput;
				ele->dpi=-1;
			}

			first=last=stack;	// empty stack

			// Find gates with indegree=0
			for(i=0; i<numberOfPrimaryInputs; i++)
			{
				ele=gates[primaryIn[i]];
				ele->changed=0;
				*last++=ele;
				primaryIn[i]=gIndex++;
			}
			for(i=0; i<numberOfFlipFlops; i++) {
				ele=gates[flipFlops[i]];
				ele->changed=0;
				*last++=ele;
				flipFlops[i]=gIndex++;
			}

			// levelize
			gIndex=0;
			numberOfGates=0;
			while(first != last)
			{
				numberOfGates++;
				ele=*first++;
				ele->index=gIndex++;
				if( ++(ele->dpi) > maxDpi) maxDpi=ele->dpi;
				for(i=0; i<ele->noutput; i++)
					if((next=ele->outlis[i]) != NULL)
						if(next->changed > 0)
						{
							if(--(next->changed) == 0) *last++=next;
							next->dpi = ele->dpi;
						}
			}

			// check for levelization
			if(numberOfGates != n)
			{
				fprintf(stderr,"Error in circuit file.\n");
				fprintf(stderr,"Some gates are not reachable from PIs.\n");
				for(i=0; i<n; i++)
				{
					ele=gates[i];
					if(ele->changed>0 || ele->dpi<0)
					{
						fprintf(stderr,"*** Unreachable gate from an input:");
						fprintf(stderr,"%d'th element %s\n",ele->index,
							ele->symbol->symbol);
					}
				}
				return -1;
			}

			for(i=0; i<numberOfPrimaryOutputs; i++)	// primaryout
				primaryOut[i]=gates[primaryOut[i]]->index;

			// sort gates by index
			for(i=0; i<n; )
			{
				if(i==gates[i]->index)
					i++;
				else 			// swap gates[i] and gates[gates[i]->index]
				{
					ele=gates[i];
					gates[i]=gates[ele->index];
					gates[ele->index]=ele;
				}
			}

			return(maxDpi+1);
	}

#endif

	int ReadableNet::addPO()
	{
		int i,j;
		Gate *gut,*last,**outlist;
		string name;

		for(i=0;i<numberOfPrimaryOutputs;i++)
		{
			gut=gates[primaryOut[i]];
			//if((last=gates[numberOfGates])==0) last=new Gate();
			//last = gates[numberOfGates];
			last = numberOfGates >= gates.size() ? new Gate() : gates[numberOfGates];

			last->index=numberOfGates;
			last->type=PO;
			last->ninput=1;
			last->fanins=new Gate*();
			last->fanins[0]=gut;
			last->noutput=0;
			last->fanouts=NULL;
#ifdef LEARNFLG
			last->pLearn.clear();
#endif
			name=gut->symbol->symbol;
			name+="_PO";

			while((last->symbol=hashTable.findHash(name,0)) != 0) name+="_PO";

			if((last->symbol=hashTable.insertHash(name,0)) == 0)
				Error::fatalerror(HASHERROR);
			else
				last->symbol->pnode=last;

			outlist=gut->fanouts;
			gut->fanouts=new Gate*[gut->noutput + 1];

			for(j=0;j<gut->noutput;j++) gut->fanouts[j]=outlist[j];
			gut->fanouts[gut->noutput]=last;
			gut->noutput+=1;

			primaryOut[i]=numberOfGates;
			if(numberOfGates >= gates.size()) {
				gates.resize(numberOfGates + 1, NULL);
			}
            gates[numberOfGates++]=last;
		}

		return(numberOfPrimaryOutputs);
	}

#ifdef INCLUDE_HOPE
	void ReadableNet::addSpareGate()
	{
		int i;
		Gate* gut;

		// add sparegates
		for(i=0;i<SIZEOFFUT;i++)
		{
			gut=new Gate();	// CONSTANT Gate
			gut->index=numberOfGates+2*i;
			gut->type=DUMMY;
			gut->ninput=0;
			gut->fanins=NULL;
			gut->noutput=1;
			gut->fanouts=new Gate*();
			gut->dpi=0;
			gut->changed=false;
			gut->symbol=NULL;
			gut->gid=0;
			gut->stem=0;
			if(gut->index + 1 >= gates.size()) {
				gates.resize(gut->index + 2); // +2 due to creation of next gate
			}
            gates[gut->index]=gut;

			gut=new Gate();   // 2-input AND, OR, XOR
			gut->index=numberOfGates+2*i+1;
			gut->type=DUMMY;
			gut->ninput=2;
			gut->fanins=new Gate*[2];
			gut->fanins[0]=gates[numberOfGates + 2 * i];
			gut->noutput=0;
			gut->fanouts=new Gate*[maxFout];
			gut->dpi=0;
			gut->changed=false;
			gut->gid=0;
			gut->stem=0;
			gut->symbol=NULL;
            gates[gut->index]=gut;    // There is alredy reserved place to this gate. See above.
		}

		// add one memory space for output list of POs
		for(i=0; i<numberOfPrimaryOutputs; i++)
		{
			gut=gates[primaryOut[i]];
			if(gut->noutput==0)
			{
				gut->fanouts=new Gate*;
				gut->fanouts[0]=0;
			}
		}
	}

#endif

#ifdef ISCAS85_NETLIST_MODE
	bool ReadableNet::circIn(::fstream *circuit)
	{
		int i,j;
		int currentline=0;
		int lineno,nfout,nfin;
		int inputs[20];
		char name[10],gtype[5],fromLine[10];
		vector<char [10]>nameList;//char nameList[MAXGATE][10];

		int lineindex[MAXLINE];

		numberOfGates=0;
		numberOfPrimaryInputs=0;
		numberOfPrimaryOutputs=0;

		//gates=new Gate*[MAXGATE];
		//primaryIn=new int[MAXPI];
		//primaryOut=new int[MAXPO];
		//headlines=new int[MAXPI];


		while(!circuit->eof())
		{
			if(circuit->peek()=='*')
				circuit->ignore(-1,'\n'); //commented line
			else
			{
				// read gate descriptions in the order of
				// line_number, label, gtype, # of fanout, # of fanin
				*circuit>>lineno;
				*circuit>>name;
				*circuit>>gtype;

				if(!strcmp(gtype,"from")) //fan-out branch
					*circuit>>fromLine;
				else
				{
					*circuit>>nfout;
					*circuit>>nfin;
				}
				circuit->ignore(-1,'\n');

				// if gate type is from, search fanin lines
				// and skip fanout lines from the gate list
				if(!strcmp(gtype,"from"))
				{
					for(i=currentline-1;i>=0;i--)
						if(!strcmp(fromLine,nameList[i]))
						{
							lineindex[lineno]=gates[i]->index;
							break;
						}
				} else //else, construct gate structures
				{
					strcpy(nameList[currentline],name); //store label
					if(currentline >= gates.size()) {
						gates.resize(currentline + 1, NULL);
					}
					gates[currentline]=new Gate;
					lineindex[lineno]=currentline;
					gates[currentline]->index=currentline; //internal netlist
					gates[currentline]->gid=lineno;	//actual netlist
					gates[currentline]->ninput=nfin;
					if(nfin!=0)	gates[currentline]->inlis=new Gate*[nfin];
					gates[currentline]->noutput=nfout;
#ifdef LEARNFLG
					gates[currentline]->plearn=0;
#endif
					if(nfout!=0)
					{
						gates[currentline]->outlis=new Gate*[nfout];
						memset(gates[currentline]->outlis,0,sizeof(Gate*)*nfout);
					}
					if(!strcmp(gtype,"inpt"))
					{
						gates[currentline]->fn=PI;
						if(numberOfPrimaryInputs >= primaryIn.size()) {
							primaryIn.resize(numberOfPrimaryInputs + 1, 0);
						}
						primaryIn[numberOfPrimaryInputs]=currentline;
						numberOfPrimaryInputs++;
					} else
					{
						if(!strcmp(gtype,"and")) gates[currentline]->fn=AND;
						else if(!strcmp(gtype,"nand")) gates[currentline]->fn=NAND;
						else if(!strcmp(gtype,"or"  )) gates[currentline]->fn=OR;
						else if(!strcmp(gtype,"nor"))  gates[currentline]->fn=NOR;
						else if(!strcmp(gtype,"not"))  gates[currentline]->fn=NAND;
						else if(!strcmp(gtype,"xor"))  gates[currentline]->fn=XOR;
						else if(!strcmp(gtype,"buff")) gates[currentline]->fn=AND;
						else if(!strcmp(gtype,"buf"))  gates[currentline]->fn=AND;
						else return false;

						// get input list
						for(i=0;i<nfin;i++) *circuit>>inputs[i];
						circuit->ignore(-1,'\n');

						// convert input index into internal and check fan-out list
						for(i=0;i<nfin;i++)
						{
							gates[currentline]->inlis[i]=gates[lineindex[inputs[i]]];
							for(j=0;j<gates[lineindex[inputs[i]]]->noutput;j++)
								if(gates[lineindex[inputs[i]]]->outlis[j]==0)
								{
									gates[lineindex[inputs[i]]]->outlis[j]=gates[currentline];
									break;
								}
						}


					}

					if(nfout==0)
					{
						if(numberOfPrimaryOutputs >= primaryOut.size()) {
							primaryOut.resize(numberOfPrimaryOutputs + 1, 0);
						}
						primaryOut[numberOfPrimaryOutputs]=currentline;
						numberOfPrimaryOutputs++;
					}
					currentline++;
				}
			}

		}

		numberOfGates=currentline;
		headlines.resize(numberOfPrimaryInputs);
		return true;
	}
#endif

}
