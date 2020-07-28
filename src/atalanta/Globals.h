// Globals.h: interface for the Globals class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ATALANTA_GLOBALS_H__
#define __ATALANTA_GLOBALS_H__

#include <list>
#include "Defines.h"

using namespace std;

#define INCLUDE_HOPE

namespace hiatpg {
	typedef struct TestVector
	{
		char *ivct;
		char *ovct;
		struct TestVector *next;
		int fltLineHash;
		int fltHash;
		int type;                    // s-a-0, s-a-1
		int index;                   // index of the fault
		int no;                     // ordinal of the vector, for -D n
		char * mask;                // mask of the covered faults (like my_FaultList)
	} TestVectorType;

	struct TestVectors
	{
		int inpVars;
		int outVars;
		int num;

		list<TestVector*> vectors;
	};

	class Globals  
	{
	public:
		//static level inVal[MAXPI];
		//static Fault *myCurrFault;

		//static int maxlevel;

		//static int maxCompact;

		//static char cctMode;
		//static char compact;
		//static char noFaultSim;
		//static char simMode;
		//static char fillMode;
		//static char learnMode;

		//static char genAllPat;

		//static int allOne;

		//static struct TestVectors tv;
#ifdef _ALG_DEBUG
		static int count;
#endif
	private:
		Globals();

	};

	const char* const fn_to_string[MAXGTYPE+3]=         /* gate function to string */
	{"AND","NAND","OR","NOR","INPUT","XOR","XNOR","DFF","DUMMY","BUFFER","NOT",
	"","","","","","","","","","PO",};

#define isWhiteSpace(c) (c==' ' || c=='\n'|| c=='\t')
#define isDelimiter(c) (c=='=' || c==',' || c=='(' || c==')')

    static bool checkBit(level word, int n) { return (word & (1<<n)) != 0; }

}
#endif // __ATALANTA_GLOBALS_H__
