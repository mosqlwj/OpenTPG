// ParralelPattern.cpp: implementation of the ParralelPattern class.
//
//////////////////////////////////////////////////////////////////////
#include <list>
#include <fstream>

#include "Error.h"

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

#include <memory>
#include "Globals.h"

namespace hiatpg {
    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    /*ParralelPattern::ParralelPattern()
    {

    }*/

    void ParralelPattern::updateAll() {
        int i, j, k;
        Gate *gut;
        Stack *prevFaultyGates = new Stack(0);

        // clear freach
        freeGates->resetFreach();
        for (i = 0; i < numberOfPrimaryInputs; i++) gates[i]->freach = false;

        // faulty gates
        j = faultyGates->getCount() - 1;
        *prevFaultyGates = *faultyGates;
        faultyGates->clear();
        for (i = 0; i <= j; i++) {
//			gut=(*faultyGates)[i];
            gut = (*prevFaultyGates)[i];
            if (gut->pFaultList.size() > 0) {
                faultyGates->push(gut);
                while (!gut->freach) {
                    gut->freach = true;
                    if (gut->noutput == 1) gut = gut->fanouts[0];
                    else if (!gut->uPath.empty()) gut = gut->uPath.front();
                }
            }
        }

        // evaluation gate list
        j = evalGates->getCount() - 1;
        evalGates->clear();
        for (i = 0; i <= j; i++) {
            gut = (*evalGates)[i];
            if (gut->freach) evalGates->push(gut);
        }


        // fault free gate list
        while (!stack->isEmpty()) {
            gut = stack->pop();
            for (i = 0; i < gut->noutput; i++)
                if (!gut->fanouts[i]->freach) {
                    gut->fanouts[i]->freach = true;
                    stack->push(gut->fanouts[i]);
                }
        }

        k = freeGates->getCount() - 1;
        freeGates->clear();
        for (i = 0; i <= k; i++) {
            gut = (*freeGates)[i];
            if (gut->freach) {
                gut->freach = false;
                freeGates->push(gut);
                for (j = 0; j < gut->ninput; j++) gut->fanins[j]->freach = false;
            }
        }

        // schedule of freach
        for (i = 0; i < numberOfPrimaryInputs; i++) gates[i]->freach = true;

        nsStack = -1;

        for (i = 0; i <= faultyGates->getCount() - 1; i++) {
            gut = (*faultyGates)[i];
            while (!gut->freach) {
                gut->freach = true;
                (*dynamicStack)[++nsStack] = gut;
                if (gut->noutput == 1) gut = gut->fanouts[0];
            }
        }
        ndStack = nsStack;
    }

    void ParralelPattern::pInitSimulation(int maxDpi) {
        int i;
        Gate *p;

        // clear changed ochange and set freach
        for (i = 0; i < numberOfGates; i++) {
            gates[i]->changed = false;
            gates[i]->freach = false;
            gates[i]->cobserve = ALL0;
            gates[i]->observe = ALL0;
        }

        // clear all sets
        for (i = 0; i < maxDpi; i++) eventList[i]->clear();
        stack->clear();
        freeGates->clear();
        faultyGates->clear();
        evalGates->clear();
        activeStems->clear();

        // initialize all stacks
        for (i = 0; i < numberOfGates; i++) {
            p = gates[i];
            if (p->pFaultList.size() > 0) faultyGates->push(p);
            if (p->noutput != 1) evalGates->push(p);
        }
        for (i = numberOfGates - 1; i >= numberOfPrimaryInputs; i--) freeGates->push(gates[i]);

        // schedule freach
        nsStack = -1;

        for (i = 0; i <= faultyGates->getCount() - 1; i++) {
            p = (*faultyGates)[i];
            while (!p->freach) {
                p->freach = true;
                (*dynamicStack)[++nsStack] = p;
                if (p->noutput == 1) p = p->fanouts[0];
            }
        }
        ndStack = nsStack;
    }

    void ParralelPattern::restoreFaultFreeValue() {
        Gate *p;

        while (!stack->isEmpty()) {
            p = stack->pop();
            p->output1 = p->output;
        }
    }

    void ParralelPattern::pFaultFreeSimulation() {
        int i;
        Gate *gut;

        for (i = freeGates->getCount() - 1; i >= 0; i--)   /*-1 je navic*/
        {
            gut = (*freeGates)[i];
            switch (gut->ninput) {
                case 1:
                    pGateEval1(gut, &gut->output1);
                    break;
                case 2:
                    pGateEval2(gut, &gut->output1);
                    break;
                case 3:
                    pGateEval3(gut, &gut->output1);
                    break;
                default:
                    pGateEval4(gut, &gut->output1);
            }
            gut->output = gut->output1;
        }
    }

    level ParralelPattern::pFaultSimulation(Gate *gut, level observe, Gate *dominator, int maxDpi) {
        int i;
        level val;

        if (observe == ALL0) return observe;
        gut->output1 ^= observe;
        stack->push(gut);
        pScheduleOutput(gut);

        if (gut->type != PO) observe = ALL0;
        if (dominator != NULL) maxDpi = dominator->dpi + 1;

        // evaluate event list
        for (i = gut->dpi + 1; i < maxDpi; i++) {
            while (!eventList[i]->isEmpty()) {
                gut = eventList[i]->pop();
                gut->changed = false;
                switch (gut->ninput) {
                    case 1:
                        pGateEval1(gut, &val);
                        break;
                    case 2:
                        pGateEval2(gut, &val);
                        break;
                    default:
                        pGateEvalX(gut, &val);
                }

                if (gut == dominator) {
                    restoreFaultFreeValue();
                    return observe | (val ^ gut->output1);
                }
                if (gut->type == PO) observe |= val ^ gut->output1;

                if (val != gut->output1) {
                    pScheduleOutput(gut);
                    gut->output1 = val;
                    stack->push(gut);
                }
            }
        }
        restoreFaultFreeValue();
        return observe;
    }

    void ParralelPattern::pGateEval1(Gate *gate, unsigned *val) {
        *val = (gate->type == NOT || gate->type == NAND || gate->type == NOR) ? ~gate->fanins[0]->output1
                                                                              : gate->fanins[0]->output1;
    }

    void ParralelPattern::pGateEval2(Gate *gate, unsigned *val) {
        switch (gate->type) {
            case AND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1;
                break;
            case NAND:
                *val = ~(gate->fanins[0]->output1 & gate->fanins[1]->output1);
                break;
            case OR:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1;
                break;
            case NOR:
                *val = ~(gate->fanins[0]->output1 | gate->fanins[1]->output1);
                break;
            case XOR:
                *val = gate->fanins[0]->output1 ^ gate->fanins[1]->output1;
                break;
            case XNOR:
                *val = ~(gate->fanins[0]->output1 ^ gate->fanins[1]->output1);
                break;
            default:
                break;
        }
    }

    void ParralelPattern::pGateEval3(Gate *gate, unsigned *val) {
        switch (gate->type) {
            case AND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1;
                break;
            case NAND:
                *val = ~(gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1);
                break;
            case OR:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1;
                break;
            default:
                *val = ~(gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1);
        }
    }

    void ParralelPattern::pGateEval4(Gate *gate, unsigned *val) {
        int cnt;
        switch (gate->type) {
            case AND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1 &
                       gate->fanins[3]->output1;
                for (cnt = 4; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
                break;
            case NAND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1 &
                       gate->fanins[3]->output1;
                for (cnt = 4; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
                *val = ~*val;
                break;
            case OR:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1 |
                       gate->fanins[3]->output1;
                for (cnt = 4; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
                break;
            default:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1 |
                       gate->fanins[3]->output1;
                for (cnt = 4; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
                *val = ~*val;
        }
    }

    void ParralelPattern::pGateEvalX(Gate *gate, unsigned *val) {
        int cnt;
        switch (gate->type) {
            case AND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1;
                for (cnt = 3; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
                break;
            case NAND:
                *val = gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1;
                for (cnt = 3; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
                *val = ~(*val);
                break;
            case OR:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1;
                for (cnt = 3; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
                break;
            case NOR:
                *val = gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1;
                for (cnt = 3; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
                *val = ~(*val);
                break;
            case XOR:
                *val = gate->fanins[0]->output1 ^ gate->fanins[1]->output1;
                break;
            case XNOR:
                *val = ~(gate->fanins[0]->output1 ^ gate->fanins[1]->output1);
                break;
        }
    }

    void ParralelPattern::pScheduleOutput(Gate *gate) {
        Gate *tempGate;
        for (int cnt = 0; cnt < gate->noutput; cnt++) {
            tempGate = gate->fanouts[cnt];
            if (!tempGate->changed) {
                eventList[tempGate->dpi]->push(tempGate);
                tempGate->changed = true;;
            }
        }
    }

    level ParralelPattern::feval(Fault *pf, Gate *gut) {
        int i;
        level val;
        Gate *g;

        g = gut->fanins[pf->line];
        if ((val = g->output1 ^ (pf->type == SA0 ? ALL0 : ALL1)) == ALL0) return val;
        if (gut->ninput == 2) {
            if (gut->type <= NAND) return val & (pf->line == 0 ? gut->fanins[1]->output1 : gut->fanins[0]->output1);
            else if (gut->type <= NOR)
                return val & (pf->line == 0 ? ~gut->fanins[1]->output1 : ~gut->fanins[0]->output1);
            else return val;
        }

        g->output1 = gut->fanins[0]->output;
        switch (gut->type) {
            case AND:
            case NAND:
                for (i = 1; i < gut->ninput; i++)
                    val &= gut->fanins[i]->output1;
                break;
            case OR:
            case NOR:
                for (i = 1; i < gut->ninput; i++)
                    val &= (~gut->fanins[i]->output1);
        }
        g->output1 = g->output;
        return val;
    }

    level ParralelPattern::pCheckFault(Gate *gut, Fault ***pf, level stemobs) {
        Fault *f;
        level observe;

        list<Fault *>::iterator current, final;

        current = gut->pFaultList.begin();
        final = gut->pFaultList.end();

        while (current != final) {
            f = *current;
            if (f->line == OUTFAULT)
                observe = gut->observe & ((f->type == SA0) ? gut->output1 : ~gut->output1);
            else    //input fault
                observe = gut->observe & feval(f, gut);

            f->observe = observe;
            if (observe != ALL0) {
                **pf = f;
                (*pf)++;
                stemobs |= observe;
            }

            current++;
        }

        return stemobs;
    }

    int ParralelPattern::pCheckPo(Gate *gut, status *flag, int nbit, int *tArray) {
        int i;
        unsigned int observe;
        int nDetect = 0;
        Fault *f;

        list<Fault *>::iterator current;
        current = gut->pFaultList.begin();

        while (current != gut->pFaultList.end()) {
            f = *current;
            observe = gut->observe;
            if (f->line == OUTFAULT)
                observe &= (f->type == SA0) ? gut->output1 : ~gut->output1;
            else
                observe &= feval(f, gut);

            if (observe != ALL0) {
                f->detected = DETECTED;
                nDetect++;
                for (i = nbit - 1; i >= 0; i--)
                    if ((observe & BITMASK[i]) != ALL0) {
                        ++tArray[i];
                        break;
                    }
                if (gut->pFaultList.size() == 1) {
                    gut->pFaultList.clear();
                    *flag = true;
                    break;
                } else {
                    current = gut->pFaultList.erase(current);
                }
            } else current++;
        }

        return nDetect;
    }

    int ParralelPattern::ftpReverse(Gate *stem, status *flag, status flag2, int nbit, int *tArray) {
        int i;
        Gate *gut, *g;
        Fault **pf;
        level observe, val;
        int nDetect = 0;

        observe = ALL0;
        pf = stem->dfault;

        stack->push(stem);

        while (!stack->isEmpty()) {
            gut = stack->pop();
            if (gut->pFaultList.size() > 0) {
                if (flag2) {
                    nDetect += pCheckPo(gut, flag, nbit, tArray);
                } else {
                    observe = pCheckFault(gut, &pf, observe);
                }
            }
            if (gut->cobserve != ALL0) observe |= gut->observe & gut->cobserve;
            if (gut->ninput == 1) {
                g = gut->fanins[0];
                if (g->noutput == 1 && g->freach) {
                    g->observe = gut->observe;
                    stack->push(g);
                }
            } else if (gut->ninput == 2) {
                g = gut->fanins[0];
                if (g->noutput == 1 && g->freach) {
                    switch (gut->type) {
                        case AND:
                        case NAND:
                            g->observe = gut->observe & gut->fanins[1]->output1;
                            break;
                        case OR:
                        case NOR:
                            g->observe = gut->observe & ~(gut->fanins[1]->output1);
                            break;
                        default:
                            g->observe = gut->observe;
                    }
                    if (g->observe != ALL0) stack->push(g);
                }

                g = gut->fanins[1];
                if (g->noutput == 1 && g->freach) {
                    switch (gut->type) {
                        case AND:
                        case NAND:
                            g->observe = gut->observe & gut->fanins[0]->output1;
                            break;
                        case OR:
                        case NOR:
                            g->observe = gut->observe & ~(gut->fanins[0]->output1);
                            break;
                        default:
                            g->observe = gut->observe;
                    }
                    if (g->observe != ALL0) stack->push(g);
                }
            } else
                for (i = 0; i < gut->ninput; i++) {
                    g = gut->fanins[i];
                    if (g->noutput == 1 && g->freach) {
                        g->output1 = ~g->output1;
                        if (gut->ninput == 1)
                            pGateEval1(gut, &val);
                        else
                            pGateEvalX(gut, &val);

                        g->observe = (val ^ gut->output1) & gut->observe;
                        g->output1 = g->output;
                        if (g->observe != ALL0) stack->push(g);
                    }
                }
        }

        *pf = 0;
        if (flag2)
            return nDetect;
        else
            return observe;
    }

    int ParralelPattern::fault1Simulation(int maxDpi, int nStem, Gate **stem, int nbit, int *tArray) {
        int i;
        Gate *gut, *g;
        Fault *f, **pf;
        int nDetect = 0;

        // step 3: fault simulation in the forward order
        activeStems->clear();


        for (i = 0; i <= evalGates->getCount() - 1; i++) {
            gut = (*evalGates)[i];
            if (!gut->freach) continue;
            gut->observe = allOne;

            if (gut->type == PO)
                nDetect += ftpReverse(gut, &updateFlag, true, nbit, tArray);
            else {
                if ((gut->observe = ftpReverse(gut, &updateFlag, false, nbit, tArray)) != ALL0) {
                    g = gut->uPath.empty() ? 0 : gut->uPath.front();
                    if ((gut->observe = pFaultSimulation(gut, gut->observe, g, maxDpi)) != ALL0) {
                        activeStems->push(gut);
                        if (g != 0) {
                            g->observe = ALL0;
                            if (g->freach)
                                g->cobserve |= gut->observe;
                            else {
                                g->freach = true;
                                (*dynamicStack)[++ndStack] = g;
                                g->cobserve = gut->observe;
                                if (g->noutput == 1) {
                                    g = g->fanouts[0];
                                    while (!g->freach) {
                                        g->freach = true;
                                        (*dynamicStack)[++ndStack] = g;
                                        g->cobserve = ALL0;
                                        if (g->noutput == 1) g = g->fanouts[0];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        // step 4: Determine grobal detectability and detected faults
        while (!activeStems->isEmpty()) {
            gut = activeStems->pop();
            if (!gut->uPath.empty()) {
                g = gut->uPath.front();
                gut->observe &= g->observe & gates[g->fos]->observe;
            }

            if (gut->observe != ALL0) {
                pf = gut->dfault;
                while ((f = (*pf)) != 0) {
                    if ((f->observe & gut->observe) != ALL0) {
                        f->observe &= gut->observe;
                        for (i = nbit - 1; i >= 0; i--)
                            if ((f->observe & BITMASK[i]) != ALL0) {
                                ++tArray[i];
                                break;
                            }
                        f->detected = DETECTED;
                        nDetect++;
                        if (f->gate->pFaultList.size() == 1) {
                            f->gate->pFaultList.clear();
                            updateFlag = true;
                        } else
                            f->gate->pFaultList.remove(f);
                    }
                    pf++;
                }
            }

        }

        return nDetect;
    }

    void ParralelPattern::updateFaultyGates() {
        int i, j;
        Gate *gut;
        Stack *prevFaultyGates = new Stack(0);


        j = faultyGates->getCount() - 1;
        *prevFaultyGates = *faultyGates;
        faultyGates->clear();

        for (i = 0; i <= j; i++) {
//			gut=(*faultyGates)[i];
            gut = (*prevFaultyGates)[i];
            if (gut->pFaultList.size() > 0) faultyGates->push(gut);
        }
    }

    void ParralelPattern::updateEvalGates() {
        int i, j;
        Gate *gut;
        Stack *prevEvalGates = new Stack(0);

        // set freach from each faulty gate to its stem
        for (i = 0; i <= faultyGates->getCount() - 1; i++) {
            gut = gates[(*faultyGates)[i]->fos];
            while (!gut->changed) {
                gut->changed = true;
                if (!gut->uPath.empty()) gut = gates[gut->uPath.front()->fos];
            }
        }

        // set evalGates based on previous stacks
        j = evalGates->getCount() - 1;
        *prevEvalGates = *evalGates;
        evalGates->clear();

        for (i = 0; i <= j; i++) {
//			gut=(*evalGates)[i];
            gut = (*prevEvalGates)[i];
            if (gut->changed) {
                evalGates->push(gut);
                gut->changed = false;
            }
        }

    }

    void ParralelPattern::updateAll1() {
        int i, j;
        Gate *gut;
        Stack *prevFaultyGates = new Stack(0);

        // clear freach
        for (i = 0; i <= freeGates->getCount() - 1; i++) {
            (*freeGates)[i]->freach = false;
        }
        for (i = 0; i < numberOfPrimaryInputs; i++)
            gates[i]->freach = false;

        // faulty gates
        j = faultyGates->getCount() - 1;
        *prevFaultyGates = *faultyGates;
        faultyGates->clear();
        nsStack = -1;

        for (i = 0; i <= j; i++) {
//			gut=(*faultyGates)[i];
            gut = (*prevFaultyGates)[i];
            if (gut->pFaultList.size() > 0) {
                faultyGates->push(gut);
                while (!gut->freach) {
                    gut->freach = true;
                    (*dynamicStack)[++nsStack] = gut;
                    if (gut->noutput == 1) gut = gut->fanouts[0];
                }
            }
        }

        // evaluation gate list
        updateEvalGates();
        ndStack = nsStack;
    }

    int ParralelPattern::fault0Simulation(int maxDpi, int nbit, int *tArray) {
        int i;
        Gate *gut, *g;
        Fault *f, **pf;
        int nDetect = 0;

        // step 1: fault free simulation
        for (i = freeGates->getCount() - 1; i >= 0; i--)   //-1 je navic
        {
            gut = (*freeGates)[i];
            switch (gut->ninput) {
                case 1:
                    pGateEval1(gut, &gut->output1);
                    break;
                case 2:
                    pGateEval2(gut, &gut->output1);
                    break;
                case 3:
                    pGateEval3(gut, &gut->output1);
                    break;
                default:
                    pGateEval4(gut, &gut->output1);
            }
            gut->output = gut->output1;
            gut->freach = false;
            gut->changed = false;
        }

        // step 2: fault dropping
        for (i = 0; i <= faultyGates->getCount() - 1; i++) {
            gut = (*faultyGates)[i];
            while (!gut->freach) {
                gut->freach = true;
                gut->cobserve = ALL0;
                if (gut->noutput == 1) gut = gut->fanouts[0];
            }
        }

        // step 3: fault simulation in the forward order
        activeStems->clear();

        for (i = 0; i <= evalGates->getCount() - 1; i++) {
            gut = (*evalGates)[i];
            if (!gut->freach) continue;
            gut->observe = allOne;
            if (gut->type == PO)
                nDetect += ftpReverse(gut, &updateFlag, true, nbit, tArray);
            else if ((gut->observe = ftpReverse(gut, &updateFlag, false, nbit, tArray)) != ALL0) {
                g = gut->uPath.empty() ? 0 : gut->uPath.front();
                if ((gut->observe = pFaultSimulation(gut, gut->observe, g, maxDpi)) != ALL0) {
                    activeStems->push(gut);
                    if (g != 0) {
                        g->observe = ALL0;
                        if (g->freach)
                            g->cobserve |= gut->observe;
                        else {
                            g->freach = true;
                            g->cobserve = gut->observe;
                            if (g->noutput == 1) {
                                g = g->fanouts[0];
                                while (!g->freach) {
                                    g->freach = true;
                                    if (g->noutput == 1) g = g->fanouts[0];
                                }
                            }
                        }
                    }
                }
            }

        }

        // step 4: determine grobal detectability and detected faults
        while (!activeStems->isEmpty()) {
            gut = activeStems->pop();
            if (!gut->uPath.empty()) {
                g = gut->uPath.front();
                gut->observe &= g->observe & gates[g->fos]->observe;
            }

            if (gut->observe != ALL0) {
                pf = gut->dfault;
                while ((f = (*pf)) != 0) {
                    if ((f->observe & gut->observe) != ALL0) {
                        f->observe &= gut->observe;
                        for (i = nbit - 1; i >= 0; i--)
                            if ((f->observe & BITMASK[i]) != ALL0) {
                                ++tArray[i];
                                break;
                            }
                        f->detected = DETECTED;
                        nDetect++;
                        if (f->gate->pFaultList.size() == 1) {
                            f->gate->pFaultList.clear();
                            updateFlag = true;
                        } else
                            f->gate->pFaultList.remove(f);
                    }
                    pf++;
                }
            }
        }

        // if event occurs, update fault free lines
        if (updateFlag) {

            updateFaultyGates();
            updateEvalGates();
            //      update_free_gates(npi);
            updateFlag = false;
        }

        return nDetect;
    }

    /*ParralelPattern::~ParralelPattern()
    {

    }*/
}
