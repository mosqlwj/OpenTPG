//
// Created by zwm on 2020/7/18.
//

#ifndef ATLANTA_PATTERNPARSER_H
#define ATLANTA_PATTERNPARSER_H

#include "AtpgEngine.h"
#include <set>

namespace hiatpg {
    class PatternParser : public AtpgEngine
    {
    private:
        vector<unordered_map<int, char>> testCubes;
        map<int, unordered_map<int, char>>  cinTestCubes;
        set<string>       testPatterns;
        string  patternPath;
        ifstream patternSorceStream;
        ifstream faultSorceStream;
        AtpgStatus atpgStatus;
        vector<unordered_map<int,char>>mergedCubes;

    public:
        PatternParser(){};
        void run(string inputMode);
        void setParams();
        void ReadPattern();
        void PatternGenerateTest();
        void CubeGenerateTest();
        void ReadFault();
        void ReadCinPattern(istream& file);
        void CoutPatternsAndFaults();
        int CinTestGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        int PatterntestGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        void PrintLog(Params& p);
        int MergeTestGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime);
        void MergeCubes();
        bool InsertCubes(unordered_map<int,char>&base,unordered_map<int,char>&cube);

    };
}



#endif //ATLANTA_PATTERNPARSER_H
