//
// Created by fish on 2020/7/11.
//

#include "AtpgEngine.h"

#include <string>
#include <sstream>
#include <list>
#include <iostream>
#include <fstream>
#include <memory>
#include <time.h>
#include <iomanip>

#include "Defines.h"
#include "Truthtable.h"
#include "Parameters.h"
#include "Error.h"

#include "Hash.h"

#include "Gate.h"
#include "Stack.h"
#include "Netlist.h"
#include "FanNet.h"
#include "ParralelPattern.h"
#include "Fault.h"

#include "Globals.h"
#include "FaultSimulation.h"
#include "ReadableNet.h"
#include "Random.h"
#include "Simulation.h"

#ifdef WIN32
#include "windows.h"
#endif

int hiatpg::AtpgEngine::testCubeGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase,
                                int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket,
                                int *nBit, double *fanTime) {
    int j, nBack;
    status faultSelectionMode;
    int lastFault;
    int state;
    int nDetect = 0;
    int profile[BITSIZE];
    bool done;
    Fault *pCurrentFault;
    Gate *gut;
    double seconds, minutes, runtime1, runtime2;

    faultSelectionMode = DEFAULTMODE;
    lastFault = numberOfFaults;
    allOne = ~(ALL1 << 1);
    done = false;

    for (int i = 0; i < numberOfFaults; i++) {
        pCurrentFault = pCurrentFault = faultList[i];
        gut = pCurrentFault->gate;

        getTime(&minutes, &seconds, &runtime1);

        // test pattern generation using fan
        state = (phase == false) ?
                fan(levels, pCurrentFault, maxBackTrack, &nBack) :
                fan1(levels, pCurrentFault, maxBackTrack, &nBack);
        (*nBackTrack) += nBack;

        getTime(&minutes, &seconds, &runtime2);
        (*fanTime) += (runtime2 - runtime1);

        if (state == TEST_FOUND) {    // fault is detected, delete the detected fault from fault list
            pCurrentFault->detected = PROCESSED;
            unordered_map<int, int> testcube;
            cout << i << '\t';
            for (j = 0; j < numberOfPrimaryInputs; j++) {
                int32_t value = gates[j]->output;
                if (value != X) {
//                    cout << "GateId: " << j << endl;
//                    cout << "Value: " << gates[j]->output << endl;
                    testcube[j] = value;
                    cout << j << " " << gates[j]->output << " ";
                }
            }
            cout << endl;
            testCubes.push_back(move(testcube));
        }
    }

    return 0;
}

int hiatpg::AtpgEngine::testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase,
                                int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket,
                                int *nBit, double *fanTime) {
    int j, nBack;
    status faultSelectionMode;
    int lastFault;
    int state;
    int nDetect = 0;
    int profile[BITSIZE];
    bool done;
    Fault *pCurrentFault;
    Gate *gut;
    double seconds, minutes, runtime1, runtime2;

    faultSelectionMode = DEFAULTMODE;
    lastFault = numberOfFaults;
    allOne = ~(ALL1 << 1);
    done = false;

    while (!done) {
        if (maxBackTrack == 0) break;

        // select any undetected and untried fault
        pCurrentFault = 0;
        switch (faultSelectionMode) {
            case CHECKPOINTMODE:
                while (--lastFault >= 0)
                    if (faultList[lastFault]->detected == UNDETECTED) {
                        pCurrentFault = faultList[lastFault];
                        gut = pCurrentFault->gate;
                        if (pCurrentFault->line != OUTFAULT)
                            gut = gut->fanins[pCurrentFault->line];
                        if (gut->isCheckPoint()) break;
                        pCurrentFault = 0;
                    }
                if (pCurrentFault == 0) {
                    faultSelectionMode = DEFAULTMODE;
                    lastFault = numberOfFaults;
                }
                break;

            default:
                while (--lastFault >= 0)
                    if (faultList[lastFault]->detected == UNDETECTED) {
                        pCurrentFault = faultList[lastFault];
                        //pCurrentFault=faultList[485];
                        break;
                    }
                if (pCurrentFault == 0) done = true;;
        }

        //printf("%d\n", pCurrentFault->index);
        if (pCurrentFault == 0) continue;
        gut = pCurrentFault->gate;

        nTestEach = 0;
        myCurrFault = pCurrentFault; //added by me

        /*      if(no_faultsim=='y') {
        printfault(test,pcurrentfault,0);
        if(logmode=='y') printfault(logfile,pcurrentfault,0);
        }
        Removed my me! */

        getTime(&minutes, &seconds, &runtime1);

        // test pattern generation using fan
        state = (phase == false) ?
                fan(levels, pCurrentFault, maxBackTrack, &nBack) :
                fan1(levels, pCurrentFault, maxBackTrack, &nBack);
        (*nBackTrack) += nBack;

        getTime(&minutes, &seconds, &runtime2);
        (*fanTime) += (runtime2 - runtime1);

        if (noFaultSim == 'y') {
            (*nTest) += nTestEach;
            if (nTestEach > 0) {
                pCurrentFault->detected = DETECTED;
                nDetect++;
            } else if (state == NO_TEST) {    // redundant faults
                pCurrentFault->detected = REDUNDANT;
                (*nRedundant)++;
            } else {    // over backtracking
                (*nOverBackTrack)++;
                pCurrentFault->detected = PROCESSED;
            }
        } else if (state == TEST_FOUND) {    // fault is detected, delete the detected fault from fault list
            pCurrentFault->detected = PROCESSED;
            // assign random zero and ones to the unassigned bits
            (*nTest)++;

            // print test cube
            unordered_map<int, int> testcube;
            for (j = 0; j < numberOfPrimaryInputs; j++) {
                int32_t value = gates[j]->output;
                if (value != X) {
//                    cout << "GateId: " << j << endl;
//                    cout << "Value: " << gates[j]->output << endl;
                    testcube[j] = value;
                }
            }
            testCubes.push_back(move(testcube));

            fillPatterns(fillMode, *nPacket, *nBit);
            for (j = 0; j < numberOfPrimaryInputs; j++) {
                gates[j]->changed = false;
                gates[j]->freach = false;
                gates[j]->cobserve = ALL0;
                gates[j]->output = gates[j]->output1;
            }

            if (++(*nBit) == maxBits) {
                *nBit = 0;
                (*nPacket)++;
            }
            stack->clear();

            // fault simulation
            profile[0] = fault0Simulation(levels, 1, profile);
            nDetect += profile[0];

            if (pCurrentFault->detected != DETECTED) {
                cout << "Error in test generation:" << pCurrentFault->index;
                cout << endl;
            }
        } else if (state == NO_TEST) {    // redundant faults
            pCurrentFault->detected = REDUNDANT;
            (*nRedundant)++;
            gut->pFaultList.remove(pCurrentFault);
            if (gut->pFaultList.empty()) updateFlag = true;
        } else {         // over backtracking
            (*nOverBackTrack)++;
            pCurrentFault->detected = PROCESSED;
        }
    }

    return nDetect;
}

using namespace hiatpg;

namespace hiatpg {
    CustomFaultlist::CustomFaultlist(int fault, Fault **faultList) : fault(fault), faultList(faultList) {
        mask = new char[fault];
        memset(mask, 0, fault);
        names = new string[fault];

        for (int i = 0; i < fault; i++) {
            if (faultList[i]->line >= 0) {
                names[i] = faultList[i]->gate->fanins[faultList[i]->line]->symbol->symbol;
                names[i] += "->";
            }
            names[i] += faultList[i]->gate->symbol->symbol;
            names[i] += fault2str[faultList[i]->type];
        }
    }

    void CustomFaultlist::printList(std::streambuf *fn) {

        if (fn != NULL) {
            ostream f(fn);
            for (int i = 0; i < fault; i++) f << names[i] << endl;
        }
    }

    void CustomFaultlist::printFaultList() {
        for (int i = 0; i < fault; i++) {
            auto pCurrentFault = faultList[i];
            string line;
            Gate *targetGate = pCurrentFault->gate;
            Gate *faninGate = nullptr;
            string strTarget = targetGate->symbol->symbol;
            string strFanin;
            int faninIndex = pCurrentFault->line;
            if (faninIndex != OUTFAULT) {
                faninGate = pCurrentFault->gate->fanins[faninIndex];
                strFanin = faninGate->symbol->symbol;
                cout << strFanin << "->" << strTarget << " /" << pCurrentFault->type << endl;
            } else {
                cout << strTarget << " /" << pCurrentFault->type << endl;
            }
        }
    }

    void CustomFaultlist::updateFaultList() {
        for (int i = 0; i < fault; i++) mask[i] = faultList[i]->detected;

        auto params = &Params::getInstance();
        fstream faultFile(params->getFaultFileName(), ios::out | ios::trunc);

        // print all fault status
        for (int i = 0; i < fault; i++) {
            auto pCurrentFault = faultList[i];
            string line;
            Gate *targetGate = pCurrentFault->gate;
            Gate *faninGate = nullptr;
            string strTarget = targetGate->symbol->symbol;
            string strFanin;
            int faninIndex = pCurrentFault->line;
            if (faninIndex != OUTFAULT) {
                faninGate = pCurrentFault->gate->fanins[faninIndex];
                strFanin = faninGate->symbol->symbol;
                faultFile << strFanin << "->" << strTarget << " /" << pCurrentFault->type << endl;
            } else {
                faultFile << strTarget << " /" << pCurrentFault->type << endl;
            }
        }

        faultFile.close();
    }

    void CustomFaultlist::writeFaultMask(std::streambuf *fn) {
        //  Writes the faltlist mask, according the order in fault
        //  0 = not detected
        //  1 = detected
        //  3 = redundant
        //  4 = aborted


        if (fn != NULL) {
            ostream file(fn);
            for (int i = 0; i < fault; i++) file << (char) (mask[i] + '0');
        }
    }

    void CustomFaultlist::writeABFaults(std::streambuf *fn) {

        if (fn != NULL) {
            ostream file(fn);

            for (int i = 0; i < fault; i++)
                if (mask[i] == 4) file << names[i] << endl;
        }
    }

    void CustomFaultlist::writeUDFaults(std::streambuf *fn) {

        if (fn != NULL) {
            ostream file(fn);

            for (int i = 0; i < fault; i++)
                if (mask[i] != 1) file << names[i] << endl;
        }
    }

    AtpgEngine::AtpgEngine() {
        inputMode = 'd';
        iseed = 0;
        faultMode = 'd';
        maxBackTrack = 10;
        maxBackTrack1 = 0;
        randomLimit = 16;
        rptMode = 'n';

        wFaults = 0;
        wTestMode = 0;
        uFaultMode = 0;
        simulationMode = 0;
        lfsrSimMode = 0;

        nTest2 = 0;
        nTest3 = 0;

        mnBit = 0;
        mnPacket = 0;
        mnTest = 0;
        mnDetect = 0;

        nRedundant = 0;

        fantime = 0;

        lid = 0;

        myCurrFault = NULL;

        benchStream = NULL;
        faultStream = NULL;
        patternStream = NULL;
        udFaultsStream = NULL;
        wFaultStream = NULL;
        maskStream = NULL;
        reportStream = NULL;
        genResStream = NULL;
    }

    void AtpgEngine::setFaults() {
        if (faultMode == 'f')
            readFaults(faultStream);
        else {
            // FSIM
            numberOfFaults = createFaultList(myNumberOfStems, myStem);
            if (numberOfFaults < 0) {
                stringstream ss;
                ss << "Fatal error: error in setting fault list";
                throw ss.str();
            }
        }
    }

    void AtpgEngine::storeLearn(Gate *gut, level val) {
        //struct LEARN *tmp;

        if (gut->ninput < 2) return;
        if (gut->numzero != lid) return;

        switch (gut->type) {
            case AND:
            case NOR:
                if (val == ZERO) return;
                break;
            case OR:
            case NAND:
                if (val == ONE) return;
                break;
            case XOR:
            case XNOR:
                break;
            default:
                return;
        }

        gut->pLearn.push_front(Learn(snode, sval, aNot(val)));

#ifdef DEBUGLEARN
        norecord++;
        learnmemory+=sizeof(struct LEARN);
        printf("learn: (node %d = %s) from ",tmp->node,level2str[tmp->tval]);
        printf("(node %d = %s)\n",gut->index,level2str[tmp->sval]);
#endif
    }

    status AtpgEngine::leval(Gate *gate) {
        int i, j;
        level val, v1;
        int numX;
        logic f;
        Gate **p;

        // forward gate evaluation
        gate->changed = false;
        p = gate->fanins;

        j = 0;

        // if a line is a head line, stop
        if (gate->isFree()) return FORWARD;

        // fault free gate evaluation
        for (i = 0; i < gate->ninput; i++)
            if (gate->fanins[i]->numzero == lid) {
                gate->numzero = lid;
                break;
            }

        gateEval1(gate, &val, &f);

        if (val == gate->output) {    // no event
            if (val != X) gate->changed = true;
            return FORWARD;
        }

        if (gate->output == X) {    // forward evaluation
            gate->output = val;                // update gate output
            stack->push(gate);
            gate->changed = true;
            scheduleOutput(gate);

            storeLearn(gate, val);
            return FORWARD;
        }

        if (val != X) return (CONFLICT);        // report conflict

        // backward implication

        switch (gate->type) {
            case AND:
            case NAND:
            case OR:
            case NOR:
                v1 = (gate->type == AND || gate->type == NOR) ? ONE : ZERO;
                if (gate->output == v1) {
                    gate->changed = true;
                    for (i = 0; i < gate->ninput; i++)
                        if (p[i]->output == X) {
                            p[i]->output = a_truthtbl1[gate->type][v1];
                            stack->push(p[i]);
                            scheduleInput(gate, i);
                        }

                    return BACKWARD;
                } else {
                    for (i = numX = 0; i < gate->ninput; i++)
                        if (p[i]->output == X) {
                            numX++;
                            j = i;
                        }

                    if (numX == 1) {
                        p[j]->output = a_truthtbl1[gate->type][gate->output];
                        gate->changed = true;
                        stack->push(p[j]);
                        scheduleInput(gate, j);
                        return BACKWARD;
                    }
                }
                break;

            case BUFF:
            case NOT:
            case PO:
                p[0]->output = a_truthtbl1[gate->type][gate->output];
                gate->changed = true;
                stack->push(p[0]);
                scheduleInput(gate, 0);
                return BACKWARD;
                break;

            case XOR:
            case XNOR:
                for (i = numX = 0; i < gate->ninput; i++)
                    if (p[i]->output == X) {
                        numX++;
                        j = i;
                    }

                if (numX == 1) {
                    v1 = (j == 0) ? p[1]->output : p[0]->output;
                    val = a_truthtbl1[gate->type][gate->output];
                    if (v1 == ONE) val = a_truthtbl1[NOT][val];
                    p[j]->output = val;
                    gate->changed = true;
                    stack->push(p[j]);
                    scheduleInput(gate, j);
                    return BACKWARD;
                }
                break;
            default:
                break;
        }
        return FORWARD;
    }

    bool AtpgEngine::impval(int maxDpi, bool backward, int last) {
        int i, start;
        status st;
        Gate *g;

        start = backward ? last : 0;

        while (true) {   // backward implication
            if (backward)
                for (i = start; i >= 0; i--)
                    while (!eventList[i]->isEmpty()) {
                        g = eventList[i]->pop();
                        if ((st = leval(g)) == CONFLICT) return false;
                    }

            // forward implication
            backward = false;
            for (i = 0; i < maxDpi; i++) {
                while (!eventList[i]->isEmpty()) {
                    if ((st = leval(eventList[i]->pop())) == CONFLICT) return false;
                    else if (st == BACKWARD) {
                        start = i - 1;
                        backward = true;
                        break;
                    }
                }
                if (backward) break;
            }
            if (!backward) break;
        }

        return true;
    }

    void AtpgEngine::learnNode(int maxDpi, int node, level val) {
        int ix;
        Gate *gut = gates[node];

        snode = node;
        gut->output = val;
        stack->push(gut);
        sval = aNot(val);

        pushEvent(gut);
        gut->numzero = ++lid;
        scheduleOutput(gut);

        if (!impval(maxDpi, FORWARD, 0)) {
            impo.push_front(Eden(node, aNot(val)));

#ifdef DEBUGLEARN
            noimpo++;
            learnmemory+=sizeof(struct EDEN);
            printf("learn: impo: node %d = %s\n",tmp->node,level2str[tmp->val]);
#endif
            for (ix = 0; ix < maxDpi; ix++)
                while (!eventList[ix]->isEmpty()) {
                    gut = eventList[ix]->pop();
                    gut->changed = false;
                }
        }

        // restore good values
        for (ix = stack->getCount() - 1; ix >= 0; ix--) {
            (*stack)[ix]->output = X;
            (*stack)[ix]->changed = false;
        }

        stack->clear();
    }

    void AtpgEngine::learn(int maxDpi) {
        int ix;
        Gate *gut;

        impo.clear();

        for (ix = 0; ix < numberOfGates; ix++) {
            gut = gates[ix];
            gut->changed = false;
            gut->numzero = -1;
            gut->output = X;

            gut->pLearn.clear();
        }

        for (ix = 0; ix < numberOfGates; ix++) {
            gut = gates[ix];
            if (gut->isFree()) continue;
            if (gut->ninput == 1) continue;

            switch (gut->type) {
                case AND:
                case NOR:
                    learnNode(maxDpi, ix, ONE);
                    if (gut->noutput > 1) learnNode(maxDpi, ix, ZERO);
                    break;
                case OR:
                case NAND:
                    learnNode(maxDpi, ix, ZERO);
                    if (gut->noutput > 1) learnNode(maxDpi, ix, ONE);
                    break;
                default:
                    if (gut->noutput > 1) {
                        learnNode(maxDpi, ix, ZERO);
                        learnNode(maxDpi, ix, ONE);
                    }
            }
        }

        stack->clear();
    }

    void AtpgEngine::initFS() {
        int i;

        setTestAbility();

        for (i = 0; i < numberOfGates; i++) {
            gates[i]->changed = false;
            gates[i]->freach = numberOfGates;
            if (gates[i]->dpi >= PPOlevel)
                cout << "Error: gut=" << gates[i]->symbol->symbol << " dpi=" << gates[i]->dpi << endl;
        }

        FanNet::setDominator(levels);
        setUniquePath(levels);

        //print_test_topic(test,nopi,nopo,name1);
        if (learnMode == 'y') learn(levels);

        for (i = 0; i < numberOfFaults; i++) {
            faultList[i]->detected = UNDETECTED;
            faultList[i]->observe = ALL0;
        }

        nRedundant = checkRedundantFaults();
        pInitSimulation(levels);

        maxDetect = numberOfFaults;
        testVectors.clear();
        testVectors1.clear();
        testStore.clear();
        testStore1.clear();
        testVectors.setSecondSize(numberOfPrimaryInputs);
        testVectors1.setSecondSize(numberOfPrimaryInputs);
        testStore.setSecondSize(numberOfPrimaryInputs);
        testStore1.setSecondSize(numberOfPrimaryInputs);

        allOne = ALL1;
    }

    void AtpgEngine::readTestFile() {
        string s;

        if (patternStream != NULL) {
            istream f(patternStream);
            testVector.num = 0;
            testVector.inpVars = numberOfPrimaryInputs;
            testVector.outVars = numberOfPrimaryOutputs;

            while (f.peek() > 0) {
                //f.getline(s, MAXPI);
                //f.get(s, MAXPI);
                f >> s;
                myCurrFault = 0;
                if (s.length() != numberOfPrimaryInputs) {
                    /*cerr<<"Fatal error: Incorrect number of test vector inputs"<<endl;
                    exit(0);*/
                    stringstream ss;
                    ss << "Fatal error: Incorrect number of test vector inputs";
                    throw ss.str();
                }
                addTestVector(&s, NULL, -1);
            }
        }
    }

    int AtpgEngine::simulateVector(string vct) {
        int i;

        inVal.clear();
        inVal.resize(numberOfPrimaryInputs);
        for (i = 0; i < numberOfPrimaryInputs; i++)
            switch (vct[i]) {
                case '0':
                    inVal[i] = ZERO;
                    break;
                case '1':
                    inVal[i] = ONE;
                    break;
                case 'x':
                case 'X':
                case '-':
                case '2':
                    inVal[i] = X;
                    break;
                default:
                    inVal[i] = X;
                    break;
            }
        return simulateHope(&mnPacket, &mnBit);
    }

    string AtpgEngine::octToBin(string *c) {

        if (numberOfPrimaryInputs / 3 + (numberOfPrimaryInputs % 3 ? 1 : 0) != c->length()) {
            stringstream ss;
            throw "Can't use it.";
        }

        string num;

        unsigned int i, p, n, lead;

        num.clear();
        num.append(numberOfPrimaryInputs, 'x');
        p = 0;
        lead = 1;
        for (i = 0; i < c->length() && p < numberOfPrimaryInputs; i++) {
            n = (*c)[i] - '0';
            if (n > 3) {
                num[p++] = '1';
                lead = 0;
                n -= 4;
            } else if (lead == 0) num[p++] = '0';
            if (n > 1) {
                num[p++] = '1';
                lead = 0;
                n -= 2;
            } else if (lead == 0) num[p++] = '0';
            if (n > 0) {
                num[p++] = '1';
                lead = 0;
            } else if (lead == 0) num[p++] = '0';
        }
        return num;
    }

    AtpgStatus AtpgEngine::getResults() {
        AtpgStatus ar;

        ar.circuitName = circuitName;
        ar.gates = numberOfGates - numberOfPrimaryInputs - numberOfPrimaryOutputs;
        ar.iv = numberOfPrimaryInputs;
        ar.ov = numberOfPrimaryOutputs;
        ar.iPatterns = nTest2;
        ar.patterns = nTest3;
        ar.faults = numberOfFaults;
        ar.detectedFaults = mnDetect;
        ar.redundantFaults = nRedundant;
        return ar;
    }

    void AtpgEngine::writeResults(AtpgStatus ar) {
        if (reportStream != NULL) {
            ostream file(reportStream);
            file.precision(3);

            file << "gates: " << ar.gates << endl;
            file << "primary input: " << ar.iv << endl;
            file << "primary output: " << ar.ov << endl;
            file << "simulate patterns: " << ar.iPatterns << endl;
            file << "final patterns: " << ar.patterns << endl;
            file << "faults: " << ar.faults << endl;
            file << "detect faults: " << ar.detectedFaults << endl;
            file << "redundant faults: " << ar.redundantFaults << endl;
            file << "test coverage:" << double(ar.detectedFaults)/double(ar.faults) << endl;
            file << "time: " << ar.time << endl;
            file.flush();
            cout << "pattern-count" << " " << ":" << " " << ar.patterns << endl;
            cout << "fault-count"<<" " << ":" << " "<< ar.faults << endl;
            cout << "pattern-coverage"<< " " << ":" << " " << fixed << std::setprecision(2) << double(ar.detectedFaults)/double(ar.faults) * 100 << "%"<< endl;
            cout << endl;
        }
    }

    void AtpgEngine::printFinalReport(AtpgStatus ar) {
        cout << "gates: " << ar.gates << endl;
        cout << "primary input: " << ar.iv << endl;
        cout << "primary output: " << ar.ov << endl;
        cout << "simulate patterns: " << ar.iPatterns << endl;
        cout << "final patterns: " << ar.patterns << endl;
        cout << "faults: " << ar.faults << endl;
        cout << "detect faults: " << ar.detectedFaults << endl;
        cout << "redundant faults: " << ar.redundantFaults << endl;
        cout << "test coverage:" << double(ar.detectedFaults)/double(ar.faults) << endl;
        cout << "time: " << ar.time << endl;
        cout.flush();
    }

    void AtpgEngine::generateTest() {
        int i;
        int nDetect3 = 0;
        status state;
        Fault *f;
        int nOverBackTrack = 0;
        int tBackTrack = 0;
        double fan1Time;
        int shuf = 0;

        testVector.num = 0;
        testVector.inpVars = numberOfPrimaryInputs;
        testVector.outVars = numberOfPrimaryOutputs;

        /******************************************************************
        *                                                                *
        *    step 3: Deterministic Test Pattern Generation Session       *
        *            (fan with unique path sensitization                 *
        *                                                                *
        ******************************************************************/
        fantime = 0;

        mnDetect += testGen(levels, BITSIZE, myNumberOfStems, myStem, maxBackTrack, false, &nRedundant, &nOverBackTrack,
                            &tBackTrack, &mnTest, &mnPacket, &mnBit, &fantime);
        nTest2 = mnTest;

        /******************************************************************
        *                                                                *
        *    step 4: Deterministic Test Pattern Generation Session       *
        *            Phase 2: Employs dynamic unique path sensitization  *
        *                                                                *
        ******************************************************************/

        state = NO_TEST;
        if (maxBackTrack1 > 0 && numberOfFaults - mnDetect - nRedundant > 0) {
            for (i = 0; i < numberOfFaults; i++) {
                f = faultList[i];
                if (f->detected == PROCESSED) f->detected = UNDETECTED;
            }

            fan1Time = 0;
            mnDetect += testGen(levels, BITSIZE, myNumberOfStems, myStem, maxBackTrack1, true, &nRedundant,
                                &nOverBackTrack, &tBackTrack, &mnTest, &mnPacket, &mnBit, &fan1Time);
            fantime += fan1Time;
        }

        nTest2 = mnTest;

        /********************************************************************
        *                                                                  *
        *       step 5: Test compaction session                            *
        *               32-bit reverse fault simulation                    *
        *               + shuffling compaction   	                       *
        *                                                                  *
        ********************************************************************/
        if (mnTest == 0) {
            nTest3 = 0;
            nDetect3 = 0;
        } else if (compact == 'n') {
            nTest3 = mnTest;
            nDetect3 = mnDetect;
        } else {
            if (maxCompact == 0) {
                compact = 'r';
            }

            // get test pattern
//            int k= mnPacket + 1;
//            while(--k>=0) {
//                for(int j=0;j<numberOfPrimaryInputs;j++)
//                    gates[j]->output1=gates[j]->output=testVectors[k][j];
//            }
//            for(i=mnBit-1;i>=0;i--) {
//                getTestVector(i);
//            }
//            printTestVector("after atpg");

            nTest3 = compactTest(levels, myNumberOfStems, myStem, &shuf, &nDetect3, mnPacket, mnBit, BITSIZE);
//            printTestVector("after atpg");
            if (nDetect3 != mnDetect) {
                /*cout<<"Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3<<endl;
                exit(0);*/
                stringstream ss;
                ss << "Error in test compaction: m_ndetect=" << mnDetect << ", ndetect3=" << nDetect3;
                throw ss.str();
            }
        }
    }

    void AtpgEngine::printTestVector(string label) {
        cout << label << endl;
        list<TestVector *>::iterator current, final;

        current = testVector.vectors.begin();
        final = testVector.vectors.end();

        while (current != final) {
            cout << (*current)->ivct << endl;
            current++;
        }
        cout << "end of pattern!" << endl;
    }

    void AtpgEngine::writeTestFile() {
        //    Writes a test file. Only test vectors (pat format).
        //    For -D n does not distinguish between vectors for the same fault - do not use here!

        list<TestVector *>::iterator current, final;

        if (patternStream != NULL) {
            ostream file(patternStream);
            file.clear();

            current = testVector.vectors.begin();
            final = testVector.vectors.end();

//            cout << "---test pattern---" << endl;
            while (current != final) {
                file << (*current)->ivct << endl;
//                cout << (*current)->ivct << endl;
                current++;
            }
        }
    }

    void AtpgEngine::writeTestFileOut() {
        //    Writes a test file. Only test vectors (pat format).
        //    For -D n does not distinguish between vectors for the same fault - do not use here!


        list<TestVector *>::iterator current, final;

        if (patternStream != NULL) {
            ostream file(patternStream);
            file.clear();

            current = testVector.vectors.begin();
            final = testVector.vectors.end();

            while (current != final) {
                file << (*current)->ivct << " " << (*current)->ovct << endl;
                current++;
            }

        }
    }

    void AtpgEngine::writeMultiTestFile() {
        //  Writes a test file with outputs. Numbers the vectors for one fault (for -D)



        list<TestVector *>::iterator current, final;

        if (patternStream != NULL) {
            ostream file(patternStream);
            file.clear();

            current = testVector.vectors.begin();
            final = testVector.vectors.end();

            while (current != final) {
                file << (*current)->no << ": " << (*current)->ivct << " " << (*current)->ovct << endl;
                current++;
            }

        }
    }

    void AtpgEngine::writeMultiTestFileMask() {
        //  Writes a test file with outputs. Numbers the vectors for one fault (for -D)



        list<TestVector *>::iterator current, final;
        TestVector *temp;

        if (patternStream != NULL) {
            ostream file(patternStream);
            file.clear();

            current = testVector.vectors.begin();
            final = testVector.vectors.end();

            while (current != final) {
                temp = *current;

                file << temp->no << ": " << temp->ivct << " " << temp->ovct << " ";

                if (temp->mask)
                    for (int i = 0; i < numberOfFaults; i++)
                        file << temp->mask[i] + '0';

                file << endl;

                current++;
            }

        }
    }

    void AtpgEngine::generateCube() {
        AtpgStatus atpgStatus;
        CustomFaultlist *customFaultlist;
        clock_t start, end;

        // parse bench
        auto p = &Params::getInstance();
        levels = parseNetlist(p->getNetlistFile());
        // read fault from std::cin
//        setFaults();
        numberOfFaults = readFaultsFromCin(myNumberOfStems,myStem);
        indexFaults();
        customFaultlist = new CustomFaultlist(numberOfFaults, faultList);
        iseed = Random::seed(iseed);

        start = clock();
        int i;
        int nDetect3 = 0;
        status state;
        Fault *f;
        int nOverBackTrack = 0;
        int tBackTrack = 0;
        double fan1Time;

        fantime = 0;
        mnDetect += testCubeGen(levels, BITSIZE, myNumberOfStems, myStem, maxBackTrack, false, &nRedundant, &nOverBackTrack,
                            &tBackTrack, &mnTest, &mnPacket, &mnBit, &fantime);

        atpgStatus = getResults();
        end = clock();
        atpgStatus.time = (end - start) / (double) CLOCKS_PER_SEC;
    }

    void AtpgEngine::createFaultlist() {
        CustomFaultlist *customFaultlist;
        auto p = &Params::getInstance();
        levels = parseNetlist(p->getNetlistFile());
        // create fault
        setFaults();
        indexFaults();
        customFaultlist = new CustomFaultlist(numberOfFaults, faultList);
        customFaultlist->printFaultList();
    }

    int AtpgEngine::run() {
        AtpgStatus atpgStatus;
        CustomFaultlist *customFaultlist;
        clock_t start, end;

        auto p = &Params::getInstance();
        levels = parseNetlist(p->getNetlistFile());

        // create fault
        setFaults();
        indexFaults();
        customFaultlist = new CustomFaultlist(numberOfFaults, faultList);

        // if read from file, print fault.
        if (wFaults) customFaultlist->printList(wFaultStream);

        iseed = Random::seed(iseed);

        start = clock();

        // reset gates status and init simulation
        initFS();

        generateTest();
        atpgStatus = getResults();
        customFaultlist->updateFaultList();

        end = clock();
        atpgStatus.time = (end - start) / (double) CLOCKS_PER_SEC;
        printFinalReport(atpgStatus);
        switch (wTestMode) {
            case 0:
                break;
            case 1:
                writeTestFile();
                break;
            case 2:
                writeTestFileOut();
                break;
            case 3:
                writeMultiTestFile();
                break;
            case 4:
                writeMultiTestFileMask();
                break;
        }

        customFaultlist->writeFaultMask(maskStream);
        if (uFaultMode == 1)
            customFaultlist->writeABFaults(udFaultsStream);
        else if (uFaultMode == 2)
            customFaultlist->writeUDFaults(udFaultsStream);

        //Close opened files
        if (benchFile.is_open()) {
            benchFile.close();
            benchStream = NULL;
        };
        if (faultFile.is_open()) {
            faultFile.close();
            faultStream = NULL;
        };
        if (patternFile.is_open()) {
            patternFile.close();
            patternStream = NULL;
        };
        if (udFaultsFile.is_open()) {
            udFaultsFile.close();
            udFaultsStream = NULL;
        };
        if (wFaultFile.is_open()) {
            wFaultFile.close();
            wFaultStream = NULL;
        };
        if (maskFile.is_open()) {
            maskFile.close();
            maskStream = NULL;
        };
        if (reportFile.is_open()) {
            reportFile.close();
            reportStream = NULL;
        };
        if (genResFile.is_open()) {
            genResFile.close();
            genResStream = NULL;
        };

        return 0;
    }

    void AtpgEngine::setParams() {
        auto p = &Params::getInstance();

        cctMode = p->getCctMode();
        randomLimit = p->getRandomLimit();
        iseed = p->getIseed();
        maxCompact = p->getMaxCompact();
        compact = p->getCompact();
        maxBackTrack = p->getMaxBackTrack();
        maxBackTrack1 = p->getMaxBackTrack1();
        if (p->getSPatternStream() != NULL) {
            patternStream = p->getSPatternStream();
        } else {
            sPatternFile = p->getSPatternFile();
        }
        learnMode = p->getLearnMode();
        faultMode = p->getFaultMode();
        if (p->getFaultStream() != NULL) {
            faultStream = p->getFaultStream();
        } else {
//            if(p->getFaultFile().length()) {
//                OpenFile(&faultFile, &faultStream, p->getFaultFile(), ios::in);
//            }
        }
//        //faultFile = p->getFaultFile();
//        if(p->getFaultStream() != NULL) {
//            faultStream = p->getFaultStream();
//        }
//        else {
//            if(p->getWFaultFile().length()) {
//                OpenFile(&wFaultFile, &wFaultStream, p->getWFaultFile(), ios::out);
//            }
//        }
        //wFaultFile = p->getWFaultFile();
        fillMode = p->getFillMode();
        genAllPat = p->getGenAllPat();
        setEachLimit(p->getEachLimit());
        noFaultSim = p->getNoFaultSim();
        uFaultMode = p->getUFaultMode();
        if (p->getUdFaultsStream() != NULL) {
            udFaultsStream = p->getUdFaultsStream();
        } else {
            if (p->getUdFaultsFile().length()) {
                OpenFile(&udFaultsFile, &udFaultsStream, p->getUdFaultsFile(), ios::out);
            }
        }
        //udFaultsFile = p->getUdFaultsFile();
        simulationMode = p->getSimulationMode();
        if (p->getMaskStream() != NULL) {
            maskStream = p->getMaskStream();
        } else {
            if (p->getMaskFile().length()) {
                OpenFile(&maskFile, &maskStream, p->getMaskFile(), ios::out);
            }
        }
        //maskFile = p->getMaskFile();
        if (p->getReportStream() != NULL) {
            reportStream = p->getReportStream();
        } else {
            if (p->getReportFile().length()) {
                OpenFile(&reportFile, &reportStream, p->getReportFile(), ios::out);
            }
        }
        //reportFile = p->getReportFile();
        wTestMode = p->getWTestMode();
        lfsrSimMode = p->getLfsrSimMode();
        lfsrPoly = p->getLfsrPoly();
        lfsrSeed = p->getLfsrSeed();
        lfsrNum = p->getLfsrNum();
    }

    void AtpgEngine::OpenFile(fstream *file, streambuf **buf, string filename, ios_base::open_mode mode) {
        file->open(filename.data(), ios_base::in | ios_base::out);
        if (!file->is_open()) {
            stringstream ss;
            ss << "Fatal error: Cannot open processed faults file: " << filename;
            throw ss.str();
        } else {
            if (buf != NULL) {
                (*buf) = file->rdbuf();
            }
            //wFaultStream = wFaultFile.rdbuf();
        }
    }
}