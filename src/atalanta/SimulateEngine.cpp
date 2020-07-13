// SimulateEngine.cpp: implementation of the SimulateEngine class.
//
//////////////////////////////////////////////////////////////////////
#include "SimulateEngine.h"

#include <fstream>
#include <list>
#include <memory>

#include "Defines.h"
#include "Error.h"
#include "FanNet.h"
#include "Fault.h"
#include "Gate.h"
#include "Netlist.h"
#include "Globals.h"
#include "Hash.h"
#include "Parameters.h"
#include "Stack.h"
#include "Truthtable.h"

namespace hiatpg {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*SimulateEngine::SimulateEngine()
{

}*/

void SimulateEngine::updateAll() {
    int i, j, k;
    Gate *gut;
    Stack *prevFaultyGates = new Stack(0);

    // clear freach
    fanNet->freeGates->resetFreach();
    for (i = 0; i < fanNet->numberOfPrimaryInputs; i++) {
        fanNet->net[i]->freach = false;
    }

    // faulty gates
    j = fanNet->faultyGates->getCount() - 1;
    *prevFaultyGates = *fanNet->faultyGates;
    fanNet->faultyGates->clear();
    for (i = 0; i <= j; i++) {
        //			gut=(*faultyGates)[i];
        gut = (*prevFaultyGates)[i];
        if (gut->pfault.size() > 0) {
            fanNet->faultyGates->push(gut);
            while (!gut->freach) {
                gut->freach = true;
                if (gut->noutput == 1)
                    gut = gut->fanouts[0];
                else if (!gut->uPath.empty())
                    gut = gut->uPath.front();
            }
        }
    }

    // evaluation gate list
    j = fanNet->evalGates->getCount() - 1;
    fanNet->evalGates->clear();
    for (i = 0; i <= j; i++) {
        gut = (*fanNet->evalGates)[i];
        if (gut->freach) fanNet->evalGates->push(gut);
    }


    // fault free gate list
    while (!fanNet->stack->isEmpty()) {
        gut = fanNet->stack->pop();
        for (i = 0; i < gut->noutput; i++) {
            if (!gut->fanouts[i]->freach) {
                gut->fanouts[i]->freach = true;
                fanNet->stack->push(gut->fanouts[i]);
            }
        }
    }

    k = fanNet->freeGates->getCount() - 1;
    fanNet->freeGates->clear();
    for (i = 0; i <= k; i++) {
        gut = (*fanNet->freeGates)[i];
        if (gut->freach) {
            gut->freach = false;
            fanNet->freeGates->push(gut);
            for (j = 0; j < gut->ninput; j++) gut->fanins[j]->freach = false;
        }
    }

    // schedule of freach
    for (i = 0; i < fanNet->numberOfPrimaryInputs; i++) {
        fanNet->net[i]->freach = true;
    }

    fanNet->nsStack = -1;

    for (i = 0; i <= fanNet->faultyGates->getCount() - 1; i++) {
        gut = (*fanNet->faultyGates)[i];
        while (!gut->freach) {
            gut->freach = true;
            (*fanNet->dynamicStack)[++fanNet->nsStack] = gut;
            if (gut->noutput == 1) {
                gut = gut->fanouts[0];
            }
        }
    }
    fanNet->ndStack = fanNet->nsStack;
}

void SimulateEngine::pInitSimulation(int maxDpi) {
    int i;
    Gate *p;

    // clear changed ochange and set freach
    for (i = 0; i < fanNet->numberOfGates; i++) {
        fanNet->net[i]->changed = false;
        fanNet->net[i]->freach = false;
        fanNet->net[i]->cobserve = ALL0;
        fanNet->net[i]->observe = ALL0;
    }

    // clear all sets
    for (i = 0; i < maxDpi; i++) {
        fanNet->eventList[i]->clear();
    }
    fanNet->stack->clear();
    fanNet->freeGates->clear();
    fanNet->faultyGates->clear();
    fanNet->evalGates->clear();
    fanNet->activeStems->clear();

    // initialize all stacks
    for (i = 0; i < fanNet->numberOfGates; i++) {
        p = fanNet->net[i];
        if (p->pfault.size() > 0) {
            fanNet->faultyGates->push(p);
        }
        if (p->noutput != 1) {
            fanNet->evalGates->push(p);
        }
    }
    for (i = fanNet->numberOfGates - 1; i >= fanNet->numberOfPrimaryInputs; i--) {
        fanNet->freeGates->push(fanNet->net[i]);
    }

    // schedule freach
    fanNet->nsStack = -1;

    for (i = 0; i <= fanNet->faultyGates->getCount() - 1; i++) {
        p = (*fanNet->faultyGates)[i];
        while (!p->freach) {
            p->freach = true;
            (*fanNet->dynamicStack)[++fanNet->nsStack] = p;
            if (p->noutput == 1) {
                p = p->fanouts[0];
            }
        }
    }
    fanNet->ndStack = fanNet->nsStack;
}

void SimulateEngine::restoreFaultFreeValue() {
    Gate *p;

    while (!fanNet->stack->isEmpty()) {
        p = fanNet->stack->pop();
        p->output1 = p->output;
    }
}

void SimulateEngine::pFaultFreeSimulation() {
    int i;
    Gate *gut;

    for (i = fanNet->freeGates->getCount() - 1; i >= 0; i--) /*-1 je navic*/
    {
        gut = (*fanNet->freeGates)[i];
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

level SimulateEngine::pFaultSimulation(Gate *gut, level observe, Gate *dominator, int maxDpi) {
    int i;
    level val;

    if (observe == ALL0) {
        return observe;
    }
    gut->output1 ^= observe;
    fanNet->stack->push(gut);
    pScheduleOutput(gut);

    if (gut->fn != PO) {
        observe = ALL0;
    }
    if (dominator != NULL) {
        maxDpi = dominator->dpi + 1;
    }

    // evaluate event list
    for (i = gut->dpi + 1; i < maxDpi; i++) {
        while (!fanNet->eventList[i]->isEmpty()) {
            gut = fanNet->eventList[i]->pop();
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
            if (gut->fn == PO) {
                observe |= val ^ gut->output1;
            }

            if (val != gut->output1) {
                pScheduleOutput(gut);
                gut->output1 = val;
                fanNet->stack->push(gut);
            }
        }
    }
    restoreFaultFreeValue();
    return observe;
}

void SimulateEngine::pGateEval1(Gate *gate, level *val) {
    *val = (gate->fn == NOT || gate->fn == NAND || gate->fn == NOR) ? ~gate->fanins[0]->output1
                                                                    : gate->fanins[0]->output1;
}

void SimulateEngine::pGateEval2(Gate *gate, level *val) {
    switch (gate->fn) {
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
    }
}

void SimulateEngine::pGateEval3(Gate *gate, level *val) {
    switch (gate->fn) {
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

void SimulateEngine::pGateEval4(Gate *gate, level *val) {
    int cnt;
    switch (gate->fn) {
        case AND:
            *val =
                    gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1 & gate->fanins[3]->output1;
            for (cnt = 4; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
            break;
        case NAND:
            *val =
                    gate->fanins[0]->output1 & gate->fanins[1]->output1 & gate->fanins[2]->output1 & gate->fanins[3]->output1;
            for (cnt = 4; cnt < gate->ninput; cnt++) *val &= gate->fanins[cnt]->output1;
            *val = ~*val;
            break;
        case OR:
            *val =
                    gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1 | gate->fanins[3]->output1;
            for (cnt = 4; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
            break;
        default:
            *val =
                    gate->fanins[0]->output1 | gate->fanins[1]->output1 | gate->fanins[2]->output1 | gate->fanins[3]->output1;
            for (cnt = 4; cnt < gate->ninput; cnt++) *val |= gate->fanins[cnt]->output1;
            *val = ~*val;
    }
}

void SimulateEngine::pGateEvalX(Gate *gate, level *val) {
    int cnt;
    switch (gate->fn) {
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

void SimulateEngine::pScheduleOutput(Gate *gate) {
    Gate *tempGate;
    for (int cnt = 0; cnt < gate->noutput; cnt++) {
        tempGate = gate->fanouts[cnt];
        if (!tempGate->changed) {
            fanNet->eventList[tempGate->dpi]->push(tempGate);
            tempGate->changed = true;
        }
    }
}

level SimulateEngine::feval(Fault *pf, Gate *gut) {
    int i;
    level val;
    Gate *g;

    g = gut->fanins[pf->line];
    if ((val = g->output1 ^ (pf->type == SA0 ? ALL0 : ALL1)) == ALL0) {
        return val;
    }
    if (gut->ninput == 2) {
        if (gut->fn <= NAND) {
            return val & (pf->line == 0 ? gut->fanins[1]->output1 : gut->fanins[0]->output1);
        } else if (gut->fn <= NOR) {
            return val & (pf->line == 0 ? ~gut->fanins[1]->output1 : ~gut->fanins[0]->output1);
        } else {
            return val;
        }
    }

    g->output1 = gut->fanins[0]->output;
    switch (gut->fn) {
        case AND:
        case NAND:
            for (i = 1; i < gut->ninput; i++) {
                val &= gut->fanins[i]->output1;
            }
            break;
        case OR:
        case NOR:
            for (i = 1; i < gut->ninput; i++) {
                val &= (~gut->fanins[i]->output1);
            }
    }
    g->output1 = g->output;
    return val;
}

level SimulateEngine::pCheckFault(Gate *gut, Fault ***pf, level stemobs) {
    Fault *f;
    level observe;

    list<Fault *>::iterator current, final;

    current = gut->pfault.begin();
    final = gut->pfault.end();

    while (current != final) {
        f = *current;
        if (f->line == OUTFAULT) {
            observe = gut->observe & ((f->type == SA0) ? gut->output1 : ~gut->output1);
        } else {  // input fault
            observe = gut->observe & feval(f, gut);
        }

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

int SimulateEngine::pCheckPo(Gate *gut, status *flag, int nbit, int *tArray) {
    int i;
    unsigned int observe;
    int nDetect = 0;
    Fault *f;

    list<Fault *>::iterator current;
    current = gut->pfault.begin();

    while (current != gut->pfault.end()) {
        f = *current;
        observe = gut->observe;
        if (f->line == OUTFAULT) {
            observe &= (f->type == SA0) ? gut->output1 : ~gut->output1;
        } else {
            observe &= feval(f, gut);
        }

        if (observe != ALL0) {
            f->detected = DETECTED;
            nDetect++;
            for (i = nbit - 1; i >= 0; i--) {
                if ((observe & BITMASK[i]) != ALL0) {
                    ++tArray[i];
                    break;
                }
            }
            if (gut->pfault.size() == 1) {
                gut->pfault.clear();
                *flag = true;
                break;
            } else {
                current = gut->pfault.erase(current);
            }
        } else {
            current++;
        }
    }

    return nDetect;
}

int SimulateEngine::ftpReverse(Gate *stem, status *flag, status flag2, int nbit, int *tArray) {
    int i;
    Gate *gut, *g;
    Fault **pf;
    level observe, val;
    int nDetect = 0;

    observe = ALL0;
    pf = stem->dfault;

    fanNet->stack->push(stem);

    while (!fanNet->stack->isEmpty()) {
        gut = fanNet->stack->pop();
        if (gut->pfault.size() > 0) {
            if (flag2) {
                nDetect += pCheckPo(gut, flag, nbit, tArray);
            } else {
                observe = pCheckFault(gut, &pf, observe);
            }
        }
        if (gut->cobserve != ALL0) {
            observe |= gut->observe & gut->cobserve;
        }
        if (gut->ninput == 1) {
            g = gut->fanins[0];
            if (g->noutput == 1 && g->freach) {
                g->observe = gut->observe;
                fanNet->stack->push(g);
            }
        } else if (gut->ninput == 2) {
            g = gut->fanins[0];
            if (g->noutput == 1 && g->freach) {
                switch (gut->fn) {
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
                if (g->observe != ALL0) {
                    fanNet->stack->push(g);
                }
            }

            g = gut->fanins[1];
            if (g->noutput == 1 && g->freach) {
                switch (gut->fn) {
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
                if (g->observe != ALL0) {
                    fanNet->stack->push(g);
                }
            }
        } else
            for (i = 0; i < gut->ninput; i++) {
                g = gut->fanins[i];
                if (g->noutput == 1 && g->freach) {
                    g->output1 = ~g->output1;
                    if (gut->ninput == 1) {
                        pGateEval1(gut, &val);
                    } else {
                        pGateEvalX(gut, &val);
                    }

                    g->observe = (val ^ gut->output1) & gut->observe;
                    g->output1 = g->output;
                    if (g->observe != ALL0) {
                        fanNet->stack->push(g);
                    }
                }
            }
    }

    *pf = 0;
    if (flag2) {
        return nDetect;
    } else {
        return observe;
    }
}

int SimulateEngine::fault1Simulation(int maxDpi, int nStem, Gate **stem, int nbit, int *tArray) {
    int i;
    Gate *gut, *g;
    Fault *f, **pf;
    int nDetect = 0;

    // step 3: fault simulation in the forward order
    fanNet->activeStems->clear();


    for (i = 0; i <= fanNet->evalGates->getCount() - 1; i++) {
        gut = (*fanNet->evalGates)[i];
        if (!gut->freach) {
            continue;
        }
        gut->observe = allOne;

        if (gut->fn == PO) {
            nDetect += ftpReverse(gut, &updateFlag, true, nbit, tArray);
        } else {
            if ((gut->observe = ftpReverse(gut, &updateFlag, false, nbit, tArray)) != ALL0) {
                g = gut->uPath.empty() ? 0 : gut->uPath.front();
                if ((gut->observe = pFaultSimulation(gut, gut->observe, g, maxDpi)) != ALL0) {
                    fanNet->activeStems->push(gut);
                    if (g != 0) {
                        g->observe = ALL0;
                        if (g->freach) {
                            g->cobserve |= gut->observe;
                        } else {
                            g->freach = true;
                            (*fanNet->dynamicStack)[++fanNet->ndStack] = g;
                            g->cobserve = gut->observe;
                            if (g->noutput == 1) {
                                g = g->fanouts[0];
                                while (!g->freach) {
                                    g->freach = true;
                                    (*fanNet->dynamicStack)[++fanNet->ndStack] = g;
                                    g->cobserve = ALL0;
                                    if (g->noutput == 1) {
                                        g = g->fanouts[0];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // step 4: Determine grobal detectability and detected faults
    while (!fanNet->activeStems->isEmpty()) {
        gut = fanNet->activeStems->pop();
        if (!gut->uPath.empty()) {
            g = gut->uPath.front();
            gut->observe &= g->observe & fanNet->net[g->fos]->observe;
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
                    if (f->gate->pfault.size() == 1) {
                        f->gate->pfault.clear();
                        updateFlag = true;
                    } else {
                        f->gate->pfault.remove(f);
                    }
                }
                pf++;
            }
        }
    }

    return nDetect;
}

void SimulateEngine::updateFaultyGates() {
    int i, j;
    Gate *gut;
    Stack *prevFaultyGates = new Stack(0);


    j = fanNet->faultyGates->getCount() - 1;
    *prevFaultyGates = *fanNet->faultyGates;
    fanNet->faultyGates->clear();

    for (i = 0; i <= j; i++) {
        //			gut=(*faultyGates)[i];
        gut = (*prevFaultyGates)[i];
        if (gut->pfault.size() > 0) {
            fanNet->faultyGates->push(gut);
        }
    }
}

void SimulateEngine::updateEvalGates() {
    int i, j;
    Gate *gut;
    Stack *prevEvalGates = new Stack(0);

    // set freach from each faulty gate to its stem
    for (i = 0; i <= fanNet->faultyGates->getCount() - 1; i++) {
        gut = fanNet->net[(*fanNet->faultyGates)[i]->fos];
        while (!gut->changed) {
            gut->changed = true;
            if (!gut->uPath.empty()) {
                gut = fanNet->net[gut->uPath.front()->fos];
            }
        }
    }

    // set evalGates based on previous stacks
    j = fanNet->evalGates->getCount() - 1;
    *prevEvalGates = *fanNet->evalGates;
    fanNet->evalGates->clear();

    for (i = 0; i <= j; i++) {
        //			gut=(*evalGates)[i];
        gut = (*prevEvalGates)[i];
        if (gut->changed) {
            fanNet->evalGates->push(gut);
            gut->changed = false;
        }
    }
}

void SimulateEngine::updateAll1() {
    int i, j;
    Gate *gut;
    Stack *prevFaultyGates = new Stack(0);

    // clear freach
    for (i = 0; i <= fanNet->freeGates->getCount() - 1; i++) {
        (*fanNet->freeGates)[i]->freach = false;
    }
    for (i = 0; i < fanNet->numberOfPrimaryInputs; i++) {
        fanNet->net[i]->freach = false;
    }

    // faulty gates
    j = fanNet->faultyGates->getCount() - 1;
    *prevFaultyGates = *fanNet->faultyGates;
    fanNet->faultyGates->clear();
    fanNet->nsStack = -1;

    for (i = 0; i <= j; i++) {
        //			gut=(*faultyGates)[i];
        gut = (*prevFaultyGates)[i];
        if (gut->pfault.size() > 0) {
            fanNet->faultyGates->push(gut);
            while (!gut->freach) {
                gut->freach = true;
                (*fanNet->dynamicStack)[++fanNet->nsStack] = gut;
                if (gut->noutput == 1) {
                    gut = gut->fanouts[0];
                }
            }
        }
    }

    // evaluation gate list
    updateEvalGates();
    fanNet->ndStack = fanNet->nsStack;
}

int SimulateEngine::fault0Simulation(int maxDpi, int nbit, int *tArray) {
    int i;
    Gate *gut, *g;
    Fault *f, **pf;
    int nDetect = 0;

    // step 1: fault free simulation
    for (i = fanNet->freeGates->getCount() - 1; i >= 0; i--)  //-1 je navic
    {
        gut = (*fanNet->freeGates)[i];
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
    for (i = 0; i <= fanNet->faultyGates->getCount() - 1; i++) {
        gut = (*fanNet->faultyGates)[i];
        while (!gut->freach) {
            gut->freach = true;
            gut->cobserve = ALL0;
            if (gut->noutput == 1) {
                gut = gut->fanouts[0];
            }
        }
    }

    // step 3: fault simulation in the forward order
    fanNet->activeStems->clear();

    for (i = 0; i <= fanNet->evalGates->getCount() - 1; i++) {
        gut = (*fanNet->evalGates)[i];
        if (!gut->freach) {
            continue;
        }
        gut->observe = allOne;
        if (gut->fn == PO) {
            nDetect += ftpReverse(gut, &updateFlag, true, nbit, tArray);
        } else if ((gut->observe = ftpReverse(gut, &updateFlag, false, nbit, tArray)) != ALL0) {
            g = gut->uPath.empty() ? 0 : gut->uPath.front();
            if ((gut->observe = pFaultSimulation(gut, gut->observe, g, maxDpi)) != ALL0) {
                fanNet->activeStems->push(gut);
                if (g != 0) {
                    g->observe = ALL0;
                    if (g->freach) {
                        g->cobserve |= gut->observe;
                    } else {
                        g->freach = true;
                        g->cobserve = gut->observe;
                        if (g->noutput == 1) {
                            g = g->fanouts[0];
                            while (!g->freach) {
                                g->freach = true;
                                if (g->noutput == 1) {
                                    g = g->fanouts[0];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // step 4: determine grobal detectability and detected faults
    while (!fanNet->activeStems->isEmpty()) {
        gut = fanNet->activeStems->pop();
        if (!gut->uPath.empty()) {
            g = gut->uPath.front();
            gut->observe &= g->observe & fanNet->net[g->fos]->observe;
        }

        if (gut->observe != ALL0) {
            pf = gut->dfault;
            while ((f = (*pf)) != 0) {
                if ((f->observe & gut->observe) != ALL0) {
                    f->observe &= gut->observe;
                    for (i = nbit - 1; i >= 0; i--) {
                        if ((f->observe & BITMASK[i]) != ALL0) {
                            ++tArray[i];
                            break;
                        }
                    }
                    f->detected = DETECTED;
                    nDetect++;
                    if (f->gate->pfault.size() == 1) {
                        f->gate->pfault.clear();
                        updateFlag = true;
                    } else {
                        f->gate->pfault.remove(f);
                    }
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


}  // namespace hiatpg
