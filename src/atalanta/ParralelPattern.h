// ParralelPattern.h: interface for the ParralelPattern class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __ATALANTA_PARRALELPATTERN_H__
#define __ATALANTA_PARRALELPATTERN_H__

#include "FanNet.h"
#include "Globals.h"
#include "Hash.h"
#include "Netlist.h"
#include "Parameters.h"
#include "Stack.h"

namespace hiatpg {
	class ParralelPattern:public FanNet
	{
	protected:
		status updateFlag;
		int allOne;

		int pCheckPo(Gate *gut,status *flag,int nbit,int *tArray);
		level pCheckFault(Gate *gut,Fault ***pf,level stemobs);

		void pGateEval1(Gate *gate,unsigned *val);
		void pGateEval2(Gate *gate,unsigned *val);
		void pGateEval3(Gate *gate,unsigned *val);
		void pGateEval4(Gate *gate,unsigned *val);
		void pGateEvalX(Gate *gate,unsigned *val);
		void pScheduleOutput(Gate *gate);

		level	feval(Fault *pf,Gate *gut);
		int		ftpReverse(Gate *stem,status *flag,status flag2, int nbit,int *tArray);
		level	pFaultSimulation(Gate *gut,level observe,Gate *dominator,int maxDpi);
		void	restoreFaultFreeValue();
		void	updateEvalGates();
		void	updateFaultyGates();

	public:
		ParralelPattern() { };

		int		fault1Simulation(int maxDpi,int nStem,Gate **stem,int nbit,int *tArray);
		int		fault0Simulation(int maxDpi,int nbit,int *tArray);
		void	pInitSimulation(int maxDpi);
		void	pFaultFreeSimulation();
		void	updateAll();
		void	updateAll1();

		virtual ~ParralelPattern() { };
    };
}
#endif // __ATALANTA_PARRALELPATTERN_H__
