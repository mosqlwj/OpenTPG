#ifndef         __ATALANTA_DEFINES_H__
#define         __ATALANTA_DEFINES_H__

namespace hiatpg {
	typedef		int	logic;
	typedef		unsigned level;

	typedef		int	boolean;
	typedef		int	status;
	typedef		int	line_type;

	const int MAXGTYPE=100;		// logic gates 
	const int MAXLEVEL=4;       // logic levels for hope
	const int ATALEVEL=5;       // logic levels for atalanta

	const int DEFAULTMODE=0;
	const int CHECKPOINTMODE=1;

	enum GateType {
        AND=0,
        NAND=1,
        OR=2,
        NOR=3,
        PI=4,
        XOR=5,
        XNOR=6,
        DFF=7,
        DUMMY=8,
        BUFF=9,
        NOT=10,
        PO=20,
        FAULTY=50
	};

	enum FaultType {
        SA0=0,               /* stuck-at fault */
        SA1=1,
        SAX=2,
        OUTFAULT=-1
	};

	enum Value {
		ZERO,
		ONE,
		X,
		D,
		DBAR
	};

    const int FAIL=	0;					//Learning
    const int PASS=1;
    const int PASS1=2;

	const int UNDETECTED=0;
	const int DETECTED=1;
	const int XDETECTED=2;               /* Potentially detected */
	const int REDUNDANT=3;
	const int PROCESSED=4;

	const int  	EMPTY=-1;
	const int  	FORWARD=0;
	const int  	BACKWARD=1;
	const int  	CONFLICT=2;
	const int  	TEST_FOUND=0;
	const int  	OVER_BACKTRACK=1;
	const int  	NO_TEST=2;

	const char ISCAS85='5';
	const char ISCAS89='9';

	const int BITSIZE=32;

#define         EOS     '\0'    /* End of string */
#define         CR      '\n'    /* carriage return */
#define         TAB     '\t'    /* tab */

#define         Z               3
#define         ALL0            0
#define         ALL1            unsigned(~0)
#define         MASK0           unsigned(~(ALL1<<1))

#define MAX(x,y) ( (x)>(y) ? (x) : (y))

	level const BITMASK[32]=
	{ MASK0, MASK0<<1, MASK0<<2, MASK0<<3,
	MASK0<<4, MASK0<<5, MASK0<<6, MASK0<<7,
	MASK0<<8, MASK0<<9, MASK0<<10, MASK0<<11,
	MASK0<<12, MASK0<<13, MASK0<<14, MASK0<<15,
	MASK0<<16, MASK0<<17, MASK0<<18, MASK0<<19,
	MASK0<<20, MASK0<<21, MASK0<<22, MASK0<<23,
	MASK0<<24, MASK0<<25, MASK0<<26, MASK0<<27,
	MASK0<<28, MASK0<<29, MASK0<<30, MASK0<<31
	};


	const char * const levelToString[MAXLEVEL+1]=      /* level to string */
	{"0","1","x","z",};

	level const parallelToLevel[2][2]=          /* parallel level types to level */
	{{X,ONE},{ZERO,Z}};

}
#endif //__ATALANTA_DEFINES_H__

