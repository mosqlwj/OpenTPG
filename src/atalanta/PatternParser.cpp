//
// Created by zwm on 2020/7/18.
//

#include "PatternParser.h"
#include <iomanip>
namespace hiatpg{
    void PatternParser::run(string inputMode) {
        CustomFaultlist *customFaultlist;
        clock_t start, end;

        levels = setBenchStream(benchStream);
        // create fault
        if (inputMode == "pattern"){
            setFaults();
            indexFaults();
            customFaultlist = new CustomFaultlist(numberOfFaults, faultList);
            if(wFaults) customFaultlist->printList(wFaultStream);
        } else{
            numberOfFaults = readFaultsFromCin(cin,myNumberOfStems,myStem);
            indexFaults();
            customFaultlist = new CustomFaultlist(numberOfFaults, faultList);
        }
        // if read from file, print fault.

        iseed=Random::seed(iseed);

        start = clock();

        // reset gates status and init simulation
        initFS();

        if (inputMode == "pattern"){
            PatternGenerateTest();
        } else {
            CubeGenerateTest();
        }
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

    void PatternParser::CubeGenerateTest()
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
        ReadCinPattern(cin);
        mnDetect+= CinTestGen(levels,BITSIZE,myNumberOfStems,myStem,maxBackTrack,false,&nRedundant,&nOverBackTrack,&tBackTrack,&mnTest,&mnPacket,&mnBit,&fantime);
//        nTest2=mnTest;

        /********************************************************************
        *                                                                  *
        *       step 5: Test compaction session                            *
        *               32-bit reverse fault simulation                    *
        *               + shuffling compaction   	                       *
        *                                                                  *
        ********************************************************************/
//        if(mnTest==0)
//        {
//            nTest3=0;
//            nDetect3=0;
//        } else if(compact=='n')
//        {
//            nTest3=mnTest;
//            nDetect3=mnDetect;
//        } else
//        {
//            if(maxCompact==0) {
//                compact='r';
//            }
//
//            nTest3= compactTest(levels,myNumberOfStems,myStem,&shuf,&nDetect3,mnPacket,mnBit,BITSIZE);
////            printTestVector("after atpg");
//            if(nDetect3 != mnDetect)
//            {
//                /*cout<<"Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3<<endl;
//                exit(0);*/
//                stringstream ss;
//                ss << "Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3;
//                throw ss.str();
//            }
//        }
        nTest3 = testPatterns.size();
        CoutPatternsAndFaults();
    }
    void PatternParser::PatternGenerateTest()
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
        mnDetect+=PatterntestGen(levels,BITSIZE,myNumberOfStems,myStem,maxBackTrack,false,&nRedundant,&nOverBackTrack,&tBackTrack,&mnTest,&mnPacket,&mnBit,&fantime);
//        nTest2=mnTest;
//
//        /********************************************************************
//        *                                                                  *
//        *       step 5: Test compaction session                            *
//        *               32-bit reverse fault simulation                    *
//        *               + shuffling compaction   	                       *
//        *                                                                  *
//        ********************************************************************/
//        if(mnTest==0)
//        {
//            nTest3=0;
//            nDetect3=0;
//        } else if(compact=='n')
//        {
//            nTest3=mnTest;
//            nDetect3=mnDetect;
//        } else
//        {
//            if(maxCompact==0) {
//                compact='r';
//            }
//
//            nTest3= compactTest(levels,myNumberOfStems,myStem,&shuf,&nDetect3,mnPacket,mnBit,BITSIZE);
////            printTestVector("after atpg");
//            if(nDetect3 != mnDetect)
//            {
//                /*cout<<"Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3<<endl;
//                exit(0);*/
//                stringstream ss;
//                ss << "Error in test compaction: m_ndetect="<<mnDetect<<", ndetect3="<<nDetect3;
//                throw ss.str();
//            }
//        }
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
        atpgStatus.patterns = testCubes.size();
//        cout << "pattern num:" << " " << atpgStatus.patterns << endl;
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
//        if(p->GetPatternPath() != "") {
//            patternPath = p->GetPatternPath();
//        }
//        else {
//            cerr << "error: pattern file path is not exist" << endl;
//        }
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
            faultFilePath = p->getFaultFile();
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

    void PatternParser::ReadCinPattern(istream& inStream)
    {
        string patternLine;
        while (getline(cin, patternLine)) {
            if (patternLine.empty()) {
                break;
            }
            unordered_map<int, char>  tempCube;
            vector<std::string> patternVec = split(patternLine," ");
            for (int i = 1; i < patternVec.size() - 1; i +=2){
                tempCube[atof(patternVec[i].c_str())] =  patternLine[i+1];
            }
            cinTestCubes[atof(patternVec[0].c_str())] = move(tempCube);
        }
    }

    int PatternParser::CinTestGen(int levels, int maxBits, int nStem, hiatpg::Gate **stem, int maxBackTrack, int phase,
                                  int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket,
                                  int *nBit, double *fanTime){
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

        auto   cinTestCubesIt = cinTestCubes.begin();
        while(cinTestCubesIt != cinTestCubes.end())
        {
            int faulIndex = (*cinTestCubesIt).first;
            if (faultList[faulIndex]->detected == DETECTED){
                cinTestCubes.erase(cinTestCubesIt);
                continue;
            }
            auto cube = (*cinTestCubesIt).second;
            for (int i = 0; i < numberOfPrimaryInputs; i++){
                auto inputIt = cube.find(i);
                if (inputIt != cube.end()){
                    gates[i]->output = static_cast<int>(atof(&((*cinTestCubesIt).second)[i]));
                } else {
                    gates[i]->output = X;
                }
            }
            fillPatterns(fillMode,*nPacket,*nBit);
            vector<int>    pattern(numberOfPrimaryInputs);
            for(j=0;j<numberOfPrimaryInputs;j++)
            {
                gates[j]->changed=false;
                gates[j]->freach=false;
                gates[j]->cobserve=ALL0;
                gates[j]->output=gates[j]->output1;
                pattern[j] = gates[j]->output & 1;
            }
            testPatterns.push_back(move(pattern));

            if(++(*nBit)==maxBits) {*nBit=0; (*nPacket)++;}
            stack->clear();

            // fault simulation
            profile[0] = fault0Simulation(levels,1,profile);
            nDetect += profile[0];
            ++cinTestCubesIt;
        }
        return nDetect;
    }

    void PatternParser::CoutPatternsAndFaults()
    {
        for (int i = 0; i < testPatterns.size(); ++i) {
            for (int j = 0; j < numberOfPrimaryInputs; ++j) {
                cout << testPatterns[i][j];
            }
            cout << endl;
        }
        for (int i = 0; i < atpgStatus.faults; i++) {
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
                cout << i << "\t" << strFanin << "->" << strTarget << " " << pCurrentFault->type<< " " << pCurrentFault->detected << endl;
            } else {
                cout << i << "\t" <<strTarget << "->" << strTarget << " " << pCurrentFault->type<< " " << pCurrentFault->detected << endl;
            }
        }
    }

    void PatternParser::ReadFault()
    {
        faultSorceStream.open(faultFilePath, ios::in);
        if (!faultSorceStream){
            cerr << "open fault file failed" << endl;
            return;
        }
        string  tempStr;
        while (getline(faultSorceStream,tempStr)){
            string sbStr = tempStr.substr(tempStr.rfind(' ') + 1, tempStr.length());
            if (sbStr == "DS"){
                atpgStatus.detectedFaults++;
            }
            if (sbStr == "RD"){
                atpgStatus.redundantFaults++;
            }
            atpgStatus.faults++;
        }
    }

    int PatternParser::PatterntestGen(int levels, int maxBits, int nStem, Gate **stem, int maxBackTrack, int phase, int *nRedundant, int *nOverBackTrack, int *nBackTrack, int *nTest, int *nPacket, int *nBit, double *fanTime)
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

    void PatternParser::PrintLog(hiatpg::Params &p)
    {
        cout << "netlist-file" << " " << ":" << " " << p.GetNetListPath() << endl;
        cout << "fault-file" << " " << ":" << " " << p.getFaultFile() << endl;
        cout << "pattern-file" << " " << ":" << " " << patternPath << endl;
        cout << "fault-count"<< " " <<":" << " " << atpgStatus.faults << endl;
        cout << "pattern-coverage" << " " << ":"<<" " << fixed << std::setprecision(2) << double(atpgStatus.detectedFaults)/double(atpgStatus.faults) * 100 << "%" << endl;
    }

}