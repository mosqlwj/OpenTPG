// Simulation.h: interface for the Simulation class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ATALANTA_SIMULATION_H__
#define __ATALANTA_SIMULATION_H__

#include "ReadableNet.h"

namespace hiatpg {
	
	class TestVectorsData {
		int secondSize;

	public:
		vector<level *> testData;

		TestVectorsData(int n_secondSize) {
			secondSize = n_secondSize;
		};

		level * operator [] (int i) { 
			int j, k, l;
			if(i >= testData.size()) {
				j = testData.size();
				testData.resize(i + 1);
				for(k = j; k <= i; k++) {
					testData[k] = new level[secondSize]; 
					for(l = 0; l < secondSize; l++) {
						testData[k][l] = 0;
					}
				}
			}
			return testData[i];
		};

		void clear(void) {
			testData.clear();
		};

		void setSecondSize(int i) {
			secondSize = i;
		};

		unsigned int size() { return testData.size(); };
	};

	class Simulation:public ReadableNet
	{
	protected:
		char compact;
		int maxCompact;
		char fillMode;
		TestVectorsData testVectors;//level testVectors[MAXTEST/10][MAXPI+1];
		TestVectorsData testStore;//level testStore[MAXTEST/10][MAXPI+1];

		void setBit(unsigned *word,int nth);
		void resetBit(unsigned *word,int nth);
		void	randomTestFsim(TestVectorsData *testSt, TestVectorsData *testVect,int pack,int noBit);
		int		reverseFsim(int levels,int nStem,Gate **stem,int *nDet,int nPacket,int nBit,int maxBits);
		int		shuffleFsim(int levels,int nStem,Gate **stem,int *nShuf,int *nDet,int nPacket,int nBit,int maxBits);
        void	fillPatternsFsim(char mode,int nPacket,int nBit);
        void printIO(int nth_bit, int start);

	public:
		Simulation(): testVectors(0), testStore(0) {
			compact = 's';
			maxCompact = 2;
			fillMode = 'r';
		};

		int compactTest(int levels,int nStem,Gate **stem,int *nShuf,int *nDet,int nPacket,int nBit,int maxBits);
		void fillPatterns(int mode,int nPacket,int nBit);

        virtual ~Simulation(){};
        string *printInputs(int nth_bit );
        string *printOutputs(int nth_bit);
	};

}
#endif // __ATALANTA_SIMULATION_H__
