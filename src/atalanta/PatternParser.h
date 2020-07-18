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

    public:
        PatternParser(){};
        void run();
        void setParams();
        void ReadPattern();
        void generateTest();
        int testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
    };
}



#endif //ATLANTA_PATTERNPARSER_H
