//
// Created by fish on 2020/7/11.
//

#ifndef ATLANTA_ATPGENGINE_H
#define ATLANTA_ATPGENGINE_H

#include <ctime>
#include <list>
#include <unordered_map>

#include "Fault.h"
#include "Parameters.h"
#include "Params.h"
#include "Random.h"
#include "ReadableNet.h"
#include "Simulation.h"

//using namespace std;

namespace hiatpg {
    class CustomFaultlist {
        int fault;
        char *mask;
        string *names;
        Fault **faultList;

    public:
        CustomFaultlist(int fault,Fault **faultList);
        void printFaultList();
        void updateFaultList();
        void printList(std::streambuf *fn);
        void writeFaultMask(std::streambuf *fn);
        void writeABFaults(std::streambuf *fn);
        void writeUDFaults(std::streambuf *fn);
    };

    struct AtpgStatus {
        AtpgStatus(): circuitName(""),          // name of the bench file
                      gates(0),                  // total gates
                      iv(0),                     // # of inputs
                      ov(0),                     // # of outputs
                      iPatterns(0),             // # of test patterns before compaction (or final, if no compaction)
                      patterns(0),               // # of final test patterns (after compaction)
                      faults(0),                 // total # of faults
                      detectedFaults(0),         // # of detected faults
                      redundantFaults(0),        // # of redundant faults
                      time(0){}
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
    protected:
        char inputMode;
        int iseed;
        char faultMode;
        string faultFile;
        int maxBackTrack;
        int maxBackTrack1;
        int randomLimit;
        char rptMode;

        int	wFaults;
        int wTestMode;
        int uFaultMode;
        int simulationMode;

        string  faultFilePath;

        //string wFaultFile;
        fstream wFaultFile;				// just for backward compatibility
        streambuf *wFaultStream;

        //string udFaultsFile;
        fstream udFaultsFile;			// just for backward compatibility
        streambuf *udFaultsStream;

        //string maskFile;
        fstream maskFile;					// just for backward compatibility
        streambuf *maskStream;

        string circuitName;

        int nRedundant;
        int mnTest;
        int mnPacket;
        int mnBit;
        int mnDetect;
        int nTest2;
        int nTest3;

        double fantime;

        void	indexFaults() {for(int i=0;i<numberOfFaults;i++) faultList[i]->index=i;};
        void	initFS();
        void readTestFile(const string &patternFileName);
        void printTestPattern(ostream &patternStream);

        void printFinalReport(AtpgStatus ar);
        void writeResults(AtpgStatus ar);
        AtpgStatus getResults();

        list<Eden> impo;
        int snode;
        level sval;
        int lid;

        bool impval(int maxDpi,bool backward,int last);
        status leval(Gate* gate);
        void learn(int maxDpi);
        void learnNode(int maxDpi,int node,level val);
        int  simulateVector(string vct);
        void storeLearn(Gate *gut,level val);
        void generateTest();
        void printTestVector(const string &label);

        string octToBin(string *c);

    public:
        AtpgEngine();
        int run();
        void generateCube();
        void setParams();
        int testCubeGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        int testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        void processFaults();
        void createFaultlist();
    };
}

#endif //ATLANTA_ATPGENGINE_H
