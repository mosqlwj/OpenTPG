// Simulation.cpp: implementation of the Simulation class.
//
//////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

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
#include "Random.h"

#include "Simulation.h"
#include "Atpg.h"

namespace hiatpg {
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    /*Simulation::Simulation()
    {

    }*/

    void Simulation::setBit(unsigned *word, int nth) {*word |= BITMASK[nth];}
    void Simulation::resetBit(unsigned *word,int nth) {*word &= ~BITMASK[nth];}
    void Simulation::setb0(unsigned *word0, unsigned *word1, int nth) { *word0 |= BITMASK[nth]; *word1 &= (~BITMASK[nth]); }
    void Simulation::setb1(unsigned *word0, unsigned *word1, int nth) { *word0 &= (~BITMASK[nth]); *word1 |= BITMASK[nth]; }
    void Simulation::setbx(unsigned *word0, unsigned *word1, int nth) { *word0 &= (~BITMASK[nth]); *word1 &= (~BITMASK[nth]); }

    int Simulation::randomFsim(int levels,int nStem,Gate **stem,level *lfsr,int limit,int maxBit,int maxDetect,int *nTest,int *nPacket,int *nBit)
    {
        int iteration=0;
        int i,j;
        int profile[BITSIZE];
        int nDetect=0;
        level value;

        while(iteration<limit)
        {
            Random::getPRandompattern(numberOfPrimaryInputs,lfsr);



            for(i=0;i<numberOfPrimaryInputs;i++) {
                gates[i]->output1= gates[i]->output=lfsr[i];
            }
            pFaultFreeSimulation();
            for(i=0;i<maxBit;i++) profile[i]=0;
            if(fault1Simulation(levels,nStem,stem,maxBit,profile)>0)
            {
                iteration=0;
                for(i=maxBit-1;i>=0;i--)
                    if(profile[i]>0)
                    {
                        (*nTest)++;
                        nDetect+=profile[i];
                        for(j=0;j<numberOfPrimaryInputs;j++) {
                            if((gates[j]->output1 & BITMASK[i]) != ALL0) {
                                setBit(&testVectors[*nPacket][j],*nBit);
                            }
                            else
                                resetBit(&testVectors[*nPacket][j],*nBit);
                        }

                        if(++(*nBit)==maxBit) {
                            *nBit=0;
                            (*nPacket)++;
                        }
                        if(compact=='n') printIO(i,*nTest);
                    }
                if(nDetect>=maxDetect) break;
            }
            else iteration++;

            for(i=0;i<=nsStack;i++) (*dynamicStack)[i]->cobserve=ALL0;
            if(updateFlag)
            {
                updateAll1();
                updateFlag=false;
            }
            else for(i=ndStack;i>nsStack;i--) (*dynamicStack)[i]->freach=0;
            ndStack=nsStack;
        }

        return nDetect;
    }

    int Simulation::randomHope(level *lfsr,int limit,int maxBit,int maxDetect,int *nTest,int *nPacket,int *nBit)
    {
        int iteration=0;
        int i,j,n,n1;
        int nDetect=0;
        int ranTest=0;

        while(iteration<limit)
        {
            Random::getPRandompattern(numberOfPrimaryInputs,lfsr);
            for(n=0, i=maxBit-1; i>=0; i--)
            {
                for(j=0;j<numberOfPrimaryInputs;j++) {
                    inVal[j]=((lfsr[j]&BITMASK[i])==ALL0)?ZERO:ONE;
                }
                goodSim(++ranTest);
                if((n1=simulation())>0)
                {
                    n+=n1;
                    (*nTest)++;
                    nDetect+=n1;
                    for(j=0;j<numberOfPrimaryInputs;j++)
                        if(inVal[j]==ONE)
                        {
                            setBit(&testVectors[*nPacket][j],*nBit);
                            resetBit(&testVectors1[*nPacket][j],*nBit);
                        } else
                        {
                            resetBit(&testVectors[*nPacket][j],*nBit);
                            setBit(&testVectors1[*nPacket][j],*nBit);
                        }
                    if(++(*nBit)==maxBit) {*nBit=0; (*nPacket)++;}
                    if(compact=='n') printIOValues(primaryIn,primaryOut);

                    if(nDetect>=maxDetect) break;
                }
            }
            iteration=(n>0) ? 0 : iteration+1;
            if(nDetect>=maxDetect) break;
        }

        return nDetect;
    }

    int Simulation::randomSim(int levels,int nStem,Gate **stem,level *lfsr,int limit,int maxBit,int maxDetect,int *nTest,int *nPacket,int *nBit)
    {
        return randomFsim(levels,nStem,stem,lfsr,limit,maxBit,maxDetect,nTest,nPacket,nBit);
    }

    int Simulation::simulateHope(int *nPacket,int *nBit)
    {
        int ranTest=0;
        int j,n1;

        goodSim(++ranTest);
        n1=simulation();

        for(j=0;j<numberOfPrimaryInputs;j++)                // ????
            if(inVal[j]==ONE)
            {
                setBit(&testVectors[*nPacket][j],*nBit);
                resetBit(&testVectors1[*nPacket][j],*nBit);
            } else
            {
                resetBit(&testVectors[*nPacket][j],*nBit);
                setBit(&testVectors1[*nPacket][j],*nBit);
            }
        return n1;
    }

    int Simulation::tGenSim(int levels,int nStem,Gate **stem,int nTest,int *profile)
    {
        return fault0Simulation(levels,1,profile);
    }

    void Simulation::fillPatternsFsim(char mode,int nPacket,int nBit)
    {
        int j,ran;

        switch(mode)
        {
            case '0':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ONE:
                            setBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ALL1;
                            break;
                        default:
                            resetBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ALL0;
                    }
                break;
            case '1':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ZERO:
                            resetBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ALL0;
                            break;
                        default:
                            resetBit(&testVectors1[nPacket][j],nBit);
                            gates[j]->output1=ALL1;
                    }
                break;
            case 'r':
            case 'x':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ZERO:
                            resetBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ALL0;
                            break;
                        case ONE:
                            setBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ALL1;
                            break;
                        default:
                            ran=(int)rand()&01;
                            if(ran!=0)
                                setBit(&testVectors[nPacket][j],nBit);
                            else
                                resetBit(&testVectors[nPacket][j],nBit);
                            gates[j]->output1=ran;
                    }
        }
    }

    void Simulation::fillPatternsHope(char mode,int nPacket,int nBit)
    {
        int j;

        switch(mode)
        {
            case '0':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ONE:
                            setb1(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ONE;
                            break;
                        default:
                            setb0(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ZERO;
                            break;
                    }
                break;
            case '1':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ZERO:
                            setb0(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ZERO;
                            break;
                        default:
                            setb1(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ONE;
                            break;
                    }
                break;
            case 'r':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ZERO:
                            setb0(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ZERO;
                            break;
                        case ONE:
                            setb1(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ONE;
                            break;
                        default:
                            if((inVal[j]=(int)rand()&01) != 0)
                            {
                                setb1(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            } else
                            {
                                setb0(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            }
                    }
                break;
            case 'x':
                for(j=0;j<numberOfPrimaryInputs;j++)
                    switch(gates[j]->output)
                    {
                        case ZERO:
                            setb0(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ZERO;
                            break;
                        case ONE:
                            setb1(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            inVal[j]=ONE;
                            break;
                        default:
                            inVal[j]=X;
                            setbx(&testVectors[nPacket][j], &testVectors1[nPacket][j],nBit);
                            break;
                    }
        }
    }

    void Simulation::fillPatterns(int mode,int nPacket,int nBit)
    {
        fillPatternsFsim(mode,nPacket,nBit);
    }

    void Simulation::randomTestFsim(TestVectorsData *testSt, TestVectorsData *testVect,
                                    int pack,int noBit)
    {
        int array[400*BITSIZE];
        int array1[400*BITSIZE];
        int i,j,x,bits,k,B,nbit=0,npacket=0;
        int maxbits=BITSIZE;

        bits=32*pack+noBit;
        for(i=0;i<bits;i++)array[i]=i;

        j=0;
        for(i=bits-1;i>=0;i--)
        {
            x=rand()%(i+1);
            array1[j]=array[x];
            array[x]=array[i];
            j++;
        }
        for(i=0;i<bits;i++)
        {
            x=array1[i];
            k=x/32;
            B=x%32;
            for(j=0;j<numberOfPrimaryInputs;j++)
                if(((*testSt)[k][j]&BITMASK[B])!=ALL0)
                    setBit(&(*testVect)[npacket][j],nbit);
                else
                    resetBit(&(*testVect)[npacket][j],nbit);
            if(++nbit==maxbits) {nbit=0;npacket++;}
        }
    }

    int Simulation::reverseFsim(int levels,int nStem,Gate **stem,int *nDet,int nPacket,int nBit,int maxBits)
    {
        int i, j, k, n;
        int nRestoredFault;
        int nDetect=0;
        int noTest=0;
        int profile[BITSIZE];

        for(i=0;i<numberOfGates;i++) gates[i]->pFaultList.clear();

        if((nRestoredFault=restoreDetectedFaultList())<0)
        {
            /*cout<<"error occurred in restoration of fault list";
            cout<<endl;
            exit(0);*/
            stringstream ss;
            ss << "error occurred in restoration of fault list";
            throw ss.str();
        }

        pInitSimulation(levels);

        updateFlag=false;
        if(nBit==0) {--nPacket; nBit=maxBits;}

        // reverse fault simulation
        k=nPacket+1;
        while(--k>=0)
        {
            if(nDetect>=nRestoredFault) break;
            if(k<nPacket) nBit=maxBits;

            allOne= (nBit==BITSIZE) ? ALL1 : ~(ALL1<<nBit);

            for(j=0;j<numberOfPrimaryInputs;j++)
                gates[j]->output1= gates[j]->output=testVectors[k][j];
            pFaultFreeSimulation();

            for(i=0;i<nBit;i++) profile[i]=0;
            if((n = fault1Simulation(levels,nStem,stem,nBit,profile))>0)
            {
                nDetect+=n;

                // print out test files
                for(i=nBit-1;i>=0;i--)
                    if(profile[i]>0)
                    {
                        noTest++;
                        if(compact=='r')
                        {
                            printIO(i,noTest );
                            /*						if(logmode=='y')
                            {
                            fprintf(logfile,"test %4d: ",no_test);
                            printinputs(logfile,nopi,i);
                            fprintf(logfile," ");
                            printoutputs(logfile,nopo,i);
                            fprintf(logfile," %4d faults detected\n",profile[i]);
                            }*/
                        }
                    }
            }

            for(i=0;i<=nsStack;i++) (*dynamicStack)[i]->cobserve=ALL0;
            if(updateFlag)
            {
                updateAll1();
                updateFlag=false;
            }
            else
                for(i=ndStack;i>nsStack;i--) (*dynamicStack)[i]->freach=0;

            ndStack=nsStack;
        }

        *nDet=nDetect;
        return(noTest);
    }

    int Simulation::shuffleFsim(int levels,int nStem,Gate **stem,int *nShuf,int *nDet,int nPacket,int nBit,int maxBits)
    {
        int i, j, k, n;
        int nRestoredFault;
        int nDetect=0;
        int noTest=0;
        int nComp=INFINITE, stop=ONE;
        int bit=0, packet=0;
        int profile[BITSIZE];
        int nArray[MAXTEST], store=0;
        bool done, flagBit;


        for(i=0;i<=maxCompact;i++) nArray[i]=0;
        done=false;
        flagBit=false;
        *nShuf=0;

        // shufle fault simulation
        if(compact=='s')
        {
            while((!done))
            {
                (*nShuf)++;
                for(i=0;i<numberOfGates;i++) gates[i]->pFaultList.clear();

                if((nRestoredFault=restoreDetectedFaultList())<0)
                {
                    /*cout<<"error occurred in restoration of fault list\n";
                    cout<<endl;
                    exit(0);*/
                    stringstream ss;
                    ss << "error occurred in restoration of fault list";
                    throw ss.str();
                }

                pInitSimulation(levels);

                updateFlag=false;
                if(flagBit)
                {
                    nBit=bit;
                    nPacket=packet;
                    //shuffles the test patterns and stores it back in the random fashion
                    randomTestFsim(&testStore, &testVectors, packet, bit);
                    bit=packet=0;
                    for(nComp=0;nComp<=maxCompact-1;nComp++)
                    {
                        stop=STOP;
                        if(nArray[nComp]!=nArray[nComp+1])
                        {
                            stop=TWO;
                            break;
                        }
                    }
                }

                noTest=0;
                nDetect=0;

                if(nBit==0) {--nPacket; nBit=maxBits;}
                k=nPacket+1;
                while(--k>=0)
                {
                    if(nDetect>=nRestoredFault) break;
                    if(k<nPacket) nBit=maxBits;
                    allOne= nBit==BITSIZE ? ALL1 : ~(ALL1<<nBit);

                    for(j=0;j<numberOfPrimaryInputs;j++)
                        gates[j]->output1= gates[j]->output=testVectors[k][j];
                    pFaultFreeSimulation();

                    for(i=0;i<nBit;i++) profile[i]=0;
                    if((n = fault1Simulation(levels,nStem,stem,nBit,profile))>0)
                    {
                        nDetect+=n;

                        // print out test files
                        for(i=nBit-1;i>=0;i--)
                            if(profile[i]>0)
                            {
                                noTest++;
                                if(stop==STOP)
                                {
                                    printIO(i,noTest );
                                    /*							    if(logmode=='y')
                                    {
                                    fprintf(logfile,"test %4d: ",no_test);
                                    printinputs(logfile,nopi,i);
                                    fprintf(logfile," ");
                                    printoutputs(logfile,nopo,i);
                                    fprintf(logfile," %4d faults detected\n",profile[i]);
                                    }*/
                                    done=true;
                                }
                                flagBit=true;
                                for(j=0;j<numberOfPrimaryInputs;j++)
                                    if((gates[j]->output1 & BITMASK[i]) != ALL0)
                                        setBit(&testStore[packet][j],bit);
                                    else
                                        resetBit(&testStore[packet][j],bit);
                                if(++bit==maxBits) {bit=0; packet++;}
                            }
                    }


                    for(i=0;i<=nsStack;i++) (*dynamicStack)[i]->cobserve=ALL0;
                    if(updateFlag)
                    {
                        updateAll1();
                        updateFlag=false;
                    }
                    else
                        for(i=ndStack;i>nsStack;i--) (*dynamicStack)[i]->freach=0;

                    ndStack=nsStack;
                }

                if(store==maxCompact+1) store=0;
                nArray[store]=noTest;
                store++;
            }
        }

        *nDet=nDetect;
        return noTest;
    }

    int Simulation::compactTest(int levels,int nStem,Gate **stem,int *nShuf,int *nDet,int nPacket,int nBit,int maxBits)
    {
        *nShuf=0;
        if(compact=='s')
            return(shuffleFsim(levels,nStem,stem,nShuf,nDet,nPacket,nBit,maxBits));
        else
            return(reverseFsim(levels,nStem,stem,nDet,nPacket,nBit,maxBits));
    }

    /*Simulation::~Simulation()
    {

    }*/
}
