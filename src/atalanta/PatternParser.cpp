//
// Created by zwm on 2020/7/18.
//

#include "PatternParser.h"
namespace hiatpg{
    void PatternParser::run() {
        AtpgStatus atpgStatus;
        CustomFaultlist *customFaultlist;
        clock_t start, end;

        levels = setBenchStream(benchStream);

        // create fault
        setFaults();
        indexFaults();
        customFaultlist = new CustomFaultlist(numberOfFaults, faultList);

        // if read from file, print fault.
        if(wFaults) customFaultlist->printList(wFaultStream);

        iseed=Random::seed(iseed);

        start = clock();

        // reset gates status and init simulation
        initFS();

        generateTest();
        atpgStatus = getResults();
        customFaultlist->updateFaultList();

        end = clock();
        atpgStatus.time = (end-start)/(double)CLOCKS_PER_SEC;
        writeResults(atpgStatus);
        switch (wTestMode)
        {
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
        if(uFaultMode == 1)
            customFaultlist->writeABFaults(udFaultsStream);
        else if (uFaultMode == 2)
            customFaultlist->writeUDFaults(udFaultsStream);

        //Close opened files
        if(benchFile.is_open()) { benchFile.close(); benchStream = NULL; };
        if(faultFile.is_open()) { faultFile.close(); faultStream = NULL; };
        if(patternFile.is_open()) { patternFile.close(); patternStream = NULL; };
        if(udFaultsFile.is_open()) { udFaultsFile.close(); udFaultsStream = NULL; };
        if(wFaultFile.is_open()) { wFaultFile.close(); wFaultStream = NULL; };
        if(maskFile.is_open()) { maskFile.close(); maskStream = NULL; };
        if(reportFile.is_open()) { reportFile.close(); reportStream = NULL; };
        if(genResFile.is_open()) { genResFile.close(); genResStream = NULL; };

        return ;
    }

    void PatternParser::generateTest()
    {
        int i;
        int nDetect3=0;
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
        fantime=0;
        ReadPattern();
        mnDetect+=testGen(levels,BITSIZE,myNumberOfStems,myStem,maxBackTrack,false,&nRedundant,&nOverBackTrack,&tBackTrack,&mnTest,&mnPacket,&mnBit,&fantime);
        nTest2=mnTest;

        /********************************************************************
        *                                                                  *
        *       step 5: Test compaction session                            *
        *               32-bit reverse fault simulation                    *
        *               + shuffling compaction   	                       *
        *                                                                  *
        ********************************************************************/
        if(mnTest==0)
        {
            nTest3=0;
            nDetect3=0;
        } else if(compact=='n')
        {
            nTest3=mnTest;
            nDetect3=mnDetect;
        } else
        {
            if(maxCompact==0) {
                compact='r';
            }

            nTest3= compactTest(levels,myNumberOfStems,myStem,&shuf,&nDetect3,mnPacket,mnBit,BITSIZE);
//            printTestVector("after atpg");
            if(nDetect3 != mnDetect)
            {
                /*cout<<"Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3<<endl;
                exit(0);*/
                stringstream ss;
                ss << "Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3;
                throw ss.str();
            }
        }
    }
    void PatternParser::ReadPattern()
    {
        patternSorceStream.open(patternPath, ios::in);
        if (!patternSorceStream){
            cerr << "open pattern file failed" << endl;
            return;
        }
        string tempString;
        while (getline(patternSorceStream,tempString)){
            unordered_map<int, char>  cube;
            for (int i = 0; i < numberOfPrimaryInputs; i++){
                cube[i] = tempString[i];
            }
            testCubes.push_back(move(cube));
        }
        cout << "Input Pattern Num:" << testCubes.size() << endl;
        return;
    }

    void PatternParser::setParams() {
        auto p = &Params::getInstance();

        cctMode = p->getCctMode();
        randomLimit = p->getRandomLimit();
        iseed = p->getIseed();
        maxCompact = p->getMaxCompact();
        compact = p->getCompact();
        maxBackTrack = p->getMaxBackTrack();
        maxBackTrack1 = p->getMaxBackTrack1();
        if(p->GetPatternPath() != "") {
            patternPath = p->GetPatternPath();
        }
        else {
            cerr << "error: pattern file path is not exist";
        }
        if(p->getBenchStream() != NULL) {
            benchStream = p->getBenchStream();
        }
        else {
            if(p->getBenchFile().length()) {
                OpenFile(&benchFile, &benchStream, p->getBenchFile(), ios::in);
            }
        }
        learnMode = p->getLearnMode();
        faultMode = p->getFaultMode();
        if(p->getFaultStream() != NULL) {
            faultStream = p->getFaultStream();
        } else {
            if(p->getFaultFile().length()) {
                OpenFile(&faultFile, &faultStream, p->getFaultFile(), ios::in);
            }
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
        if(p->getUdFaultsStream() != NULL) {
            udFaultsStream = p->getUdFaultsStream();
        }
        else {
            if(p->getUdFaultsFile().length()) {
                OpenFile(&udFaultsFile, &udFaultsStream, p->getUdFaultsFile(), ios::out);
            }
        }
        //udFaultsFile = p->getUdFaultsFile();
        simulationMode = p->getSimulationMode();
        if(p->getMaskStream() != NULL) {
            maskStream = p->getMaskStream();
        }
        else {
            if(p->getMaskFile().length()) {
                OpenFile(&maskFile, &maskStream, p->getMaskFile(), ios::out);
            }
        }
        //maskFile = p->getMaskFile();
        if(p->getReportStream() != NULL) {
            reportStream = p->getReportStream();
        }
        else {
            if(p->getReportFile().length()) {
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

    int PatternParser::testGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime)
    {
        int j,nBack;
        status faultSelectionMode;
        int lastFault;
        int state;
        int nDetect=0;
        int profile[BITSIZE];
        bool done;
        Fault *pCurrentFault;
        Gate *gut;
        double seconds,minutes,runtime1,runtime2;

        faultSelectionMode=DEFAULTMODE;
        lastFault=numberOfFaults;
        allOne=~(ALL1<<1);
        done=false;

        auto   testCubesIt = testCubes.begin();
        while(testCubesIt != testCubes.end())
        {
            for (int i = 0; i < numberOfPrimaryInputs; i++){
                gates[i]->output = static_cast<int>(atof(&(*testCubesIt)[i]));
            }
            fillPatterns(fillMode,*nPacket,*nBit);
            for(j=0;j<numberOfPrimaryInputs;j++)
            {
                gates[j]->changed=false;
                gates[j]->freach=false;
                gates[j]->cobserve=ALL0;
                gates[j]->output=gates[j]->output1;
            }

            if(++(*nBit)==maxBits) {*nBit=0; (*nPacket)++;}
            stack->clear();

            // fault simulation
            profile[0] = fault0Simulation(levels,1,profile);
            nDetect += profile[0];
            ++testCubesIt;
        }
        return nDetect;
    }
}