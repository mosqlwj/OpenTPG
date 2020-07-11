//
// Created by fish on 2020/7/11.
//

#ifndef ATLANTA_ATPGENGINE_H
#define ATLANTA_ATPGENGINE_H

#include <list>
#include <ctime>
#include <unordered_map>

#include "Fault.h"
#include "Simulation.h"
#include "Parameters.h"
#include "Params.h"
#include "Random.h"
#include "ReadableNet.h"

using namespace std;

namespace hiatpg {
    class CustomFaultlist {
        int fault;
        char *mask;
        string *names;
        Fault **faultList;

    public:
        CustomFaultlist(int fault,Fault **faultList);
        void updateFaultList();
        void printList(std::streambuf *fn);
        void writeFaultMask(std::streambuf *fn);
        void writeABFaults(std::streambuf *fn);
        void writeUDFaults(std::streambuf *fn);
    };

    struct AtpgStatus {
        string circuitName;          // name of the bench file
        int gates;                  // total gates
        int iv;                     // # of inputs
        int ov;                     // # of outputs
        int iPatterns;             // # of test patterns before compaction (or final, if no compaction)
        int patterns;               // # of final test patterns (after compaction)
        int faults;                 // total # of faults
        int detectedFaults;         // # of detected faults
        int redundantFaults;        // # of redundant faults
        double time;                // computational time
    };

    class AtpgEngine : public Simulation
    {
    private:
        vector<unordered_map<int, int>> testCubes;

        char inputMode;
        int iseed;
        char faultMode;
        int maxBackTrack;
        int maxBackTrack1;
        int randomLimit;
        char rptMode;

        int	wFaults;
        int wTestMode;
        int uFaultMode;
        int simulationMode;

        fstream genResFile;
        streambuf *genResStream;

        string sPatternFile;				// it must be here cause this file is I/O and it is opened after parameters have been processed
        fstream patternFile;			// just for backward compatibility
        streambuf *patternStream;

        //string benchFile;
        fstream benchFile;				// just for backward compatibility
        streambuf *benchStream;

        //string faultFile;
        fstream faultFile;				// just for backward compatibility
        streambuf *faultStream;

        //string wFaultFile;
        fstream wFaultFile;				// just for backward compatibility
        streambuf *wFaultStream;

        //string udFaultsFile;
        fstream udFaultsFile;			// just for backward compatibility
        streambuf *udFaultsStream;

        //string maskFile;
        fstream maskFile;					// just for backward compatibility
        streambuf *maskStream;

        //string reportFile;
        fstream reportFile;
        streambuf *reportStream;	// just for backward compatibility

        string circuitName;

        string lfsrPoly;
        string lfsrSeed;
        int lfsrNum;
        int lfsrSimMode;

        int maxDetect;
        int nRedundant;
        int mnTest;
        int mnPacket;
        int mnBit;
        int mnDetect;
        int nTest2;
        int nTest3;

        double fantime;

//		TestVectors tv;

        //fstream circuit;

        //Simulation simulation;

        void	help();
        void	indexFaults() {for(int i=0;i<numberOfFaults;i++) faultList[i]->index=i;};
        void	initFS();
        int		optionSet(int argc,char **argv);
        int		readOption(char option,char **array,int i,int n);
        void printTestVector(string label);
        void	readTestFile();
        void	writeTestFile();
        void	writeTestFileOut();
        void	writeMultiTestFile();
        void	writeMultiTestFileMask();
        void	setFaults();

        void	writeResults(AtpgStatus ar);
        AtpgStatus getResults();

        list<Eden> impo;
        int snode;
        level sval;
        int lid;

        bool impval(int maxDpi,bool backward,int last);
        status leval(Gate* gate);
        void learn(int maxDpi);
        void learnNode(int maxDpi,int node,level val);
        int  simulateLFSR();
        int  simulateTest();
        int  simulateVector(string vct);
        void simulateAllVectors();
        void storeLearn(Gate *gut,level val);
        void generateTest();

        string octToBin(string *c);
        int generatePolyAndSeed(void);
        void OpenFile(fstream *file, streambuf **buf, string filename, ios_base::open_mode);

    public:
        AtpgEngine();
        int run(int argc, char **argv);
        int run();
        void setParams();

        int testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
    };
}

#endif //ATLANTA_ATPGENGINE_H
