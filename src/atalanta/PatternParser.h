//
// Created by zwm on 2020/7/18.
//

#ifndef ATLANTA_PATTERNPARSER_H
#define ATLANTA_PATTERNPARSER_H

#include "AtpgEngine.h"
namespace hiatpg {
    class PatternParser : public AtpgEngine
    {
    private:
        vector<unordered_map<int, char>> testCubes;
        string  patternPath;
        ifstream patternSorceStream;
        ifstream faultSorceStream;
        AtpgStatus atpgStatus;

    public:
        PatternParser(){};
        void run();
        void setParams();
        void ReadPattern();
        void generateTest();
        void ReadFault();
        int testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        void PrintLog(Params& p);
    };
}



#endif //ATLANTA_PATTERNPARSER_H
