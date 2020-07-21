#ifndef __ATALANTA_PARAMS_H__
#define __ATALANTA_PARAMS_H__

#include <dirent.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>

#include <sstream>
#include <string>

#include "Defines.h"
#include "cmdline.h"
//#include "Fault.cpp"
#include <fstream>

using namespace std;


#ifdef _WIN32
#ifndef PATH_MAX
#define PATH_MAX 260
#endif  //
extern const char *realpath(const char *restrict file_name, char *restrict resolved_name);
#endif

namespace hiatpg {
class Params {
private:
    cmdline::parser options;

protected:
    char cctMode;
    int randomLimit;
    unsigned int iseed;
    int maxCompact;
    char compact;
    int maxBackTrack;
    int maxBackTrack1;
    string sPatternFile;
    streambuf *sPatternStream;
    char learnMode;
    char faultMode;
    string faultFile;
    char simMode;
    char fillMode;
    char genAllPat;
    int eachLimit;
    char noFaultSim;
    string udFaultsFile;
    streambuf *udFaultsStream;
    int uFaultMode;
    string wFaultFile;
    streambuf *wFaultStream;
    int simulationMode;
    string maskFile;
    streambuf *maskStream;
    string reportFile;
    streambuf *reportStream;
    int wTestMode;
    int lfsrSimMode;
    string lfsrPoly;
    string lfsrSeed;
    int lfsrNum;
    fstream pat;
    fstream report;
    fstream fault;
    string execAction;    //  执行什么动作
    string netlistFile;

public:
    const string &getNetlistFile() const { return netlistFile; }

protected:
    //  网表文件地址
    string cacheAddress;  //  网表文件存放在redis时,在redis上的地址
    string target;        // 目录
    string patternPath;   // pattern文件

    Params(void) {
        // 加入指定类型的输入參数
        // 第一个參数：长名称
        // 第二个參数：短名称（‘\0‘表示没有短名称）
        // 第三个參数：參数描写叙述
        // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
        // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
        options.add<string>("exec", 'e',
                            "The function need to be execute, currently we support "
                            "'create-fault' 'upload-netlist' 'atpg' 'simulate' 'stat'",
                            true, "");
        options.add<string>("netlist", 'n',
                            "The netlist file name, support local file and redis address, "
                            "such as 'file:~/c17.bench' and 'tcp://192.168.1.101:2345/NetList'",
                            false, "");
        options.add<string>("cache", 'c', "The url where the netlist saved in redi", false, "");
        options.add<string>("fault", 'f', "fault file name", false, "");
        options.add<string>("target", 't', "The directory of statistics input files", false, "");
        cctMode = ISCAS89;
        randomLimit = 16;
        iseed = 0;
        maxCompact = 2;
        compact = 's';
        maxBackTrack = 10;
        maxBackTrack1 = 0;
        sPatternFile = "";
        sPatternStream = NULL;
        learnMode = 'n';
        faultMode = 'd';
        faultFile = "";
        simMode = 'f';
        fillMode = 'r';
        genAllPat = 'n';
        eachLimit = 0;
        noFaultSim = 'n';
        udFaultsFile = "";
        udFaultsStream = NULL;
        uFaultMode = 0;
        wFaultFile = "";
        wFaultStream = NULL;
        simulationMode = 0;
        maskFile = "";
        maskStream = NULL;
        reportFile = "";
        reportStream = NULL;
        wTestMode = 0;
        lfsrSimMode = 0;
        lfsrPoly = "";
        lfsrSeed = "";
        lfsrNum = 1000;
    };

public:
    static Params &getInstance() {
        static Params instance;
        return instance;
    }
    int showHelp(int argc, char *argv[]) {
        std::printf(
            "Usage:\n"
            "   atalanta --exec create-fault   --netlist <URI>\n"
            "   atalanta --exec upload-netlist --netlist <FILE> --cache <URI>\n"
            "   atalanta --exec atpg           --netlist <URI>\n"
            "   atalanta --exec simulate       --netlist <URI>\n"
            "   atalanta --exec stat           --target <DIR>\n"
            "   atalanta --help|-h\n"
            "\n"
            "Options:\n"
            "   Notice: We support the short command mode. And the leader letter of the short command is just the \n"
            "           first character of the long command. Such as, '-e' is the short command of '--exec', \n"
            "           and '-n' is the short command of the '--netlist', and so on.\n"
            "\n"
            "   --exec  <ACTION>    Give the application a guide for which action will be executed.\n"
            "                       The <ACTION> should be one of the below:.\n"
            "                         'create-fault'   :  Create fault from the specified file. The fault \n"
            "                                             infomations will be print to stdout.\n"
            "                         'upload-netlist' :  Parse the netlist file and upload netlist informations to \n"
            "                                             the cache service(That means the redis-server)\n"
            "                         'atpg'           :  Execute the ATPG function. The cube will be write to the \n"
            "                                             stdout.\n"
            "                         'simulate'       :  Execute the simulate function. The fault status and the \n"
            "                                             pattern will be print to the stdout.\n"
            "                         'stat'           :  Make the statistics from the file in the directory of \n"
            "                                             <DIR>.\n"
            "   --netlist <URI>     To specified where to read the netlist. If you want to let the aplication read\n"
            "                       netlist from file, you just need give the file name, or add 'file:' as the \n"
            "                       prefix. Such as 'file:/home/user/benchs/c17.bench'. If you want to let the \n"
            "                       appilcation read netlist from the cache service. You need a URI like address. \n"
            "                       Such as 'tcp://192.168.1.110:3678/NETLIST'\n"
            "   --cache <URI>       The address of the cache server.\n"
            "   --target <DIR>      Where are the files placed for making the final statistics. There are several \n"
            "                       kind  of files in <DIR>:\n"
            "                         'xxx.bench'   :   The netlist file.\n"
            "                         'xxx.fault'   :   The fault list file.\n"
            "                         'xxx.pattern' :   The pattern list file.\n"
            "                         'xxx.status'  :   The fault status file.\n"
            "   --help|-h           Show this help.\n"
            "\n"
            "Example:\n"
            "   1. Create faults from the netlist file\n"
            "       atalanta --exec create-fault --netlist 'file:./test/c17.bench' > ./test/c17.fault\n"
            "\n"
            "   2. Run ATPG or generate the cubes\n"
            "       cat ./test/c17.fault | atalanta --exec atpg --netlist 'file:./test/c17.bench' > ./test/c17.cubes\n"
            "\n"
            "   3. Do simulate for the cubes\n"
            "       cat ./test/c17.cubes | atalanta --exec simulate --netlist file:./test/c17.bench > "
            "./test/c17.pattern-status\n"
            "\n"
            "   4. Split the patterns and fault status\n"
            "       cat ./test/c17.pattern-status | grep 'fault-status:' > ./test/c17.status\n"
            "       cat ./test/c17.pattern-status | grep 'pattern:' | awk '{print $2}' > ./test/c17.pattern\n"
            "\n"
            "   5. Show the pattern coverage\n"
            "       atalanta --exec stat --target ./test\n");

        return 0;
    }

    void ParserTargetPath() {
        if (target.empty()) {
            return;
        }
        DIR *dp;
        struct dirent *dirp;
        //            string path = "../print/";
        if ((dp = opendir(target.c_str())) == NULL) {
            cerr << "target path wrong" << endl;
        }
        while ((dirp = readdir(dp)) != NULL) {
            if (strcmp(".", dirp->d_name) == 0 || strcmp("..", dirp->d_name) == 0) {
                continue;
            }
            //                cout << dirp->d_name << endl;
            string dName = string(dirp->d_name);
            string subStr = dName.substr(dName.find('.') + 1, dName.length());
            if (subStr.size() == 0) {
                continue;
            }
            if (subStr == "fault") {
                faultFile = target + string(dirp->d_name);
                continue;
            }
            if (subStr == "pattern") {
                patternPath = target + string(dirp->d_name);
            }
        }
        closedir(dp);
    }

    void parseCheck(int argc, char *argv[]) {
        if ((argc > 1) && ((0 == strcmp(argv[1], "-h")) || (0 == strcmp(argv[1], "--help")))) {
            showHelp(argc, argv);
            return;
        }
        options.parse_check(argc, argv);
        execAction = options.get<string>("exec");
        netlistFile = options.get<string>("netlist");
        cacheAddress = options.get<string>("cache");
        target = options.get<string>("target");

        string pureName = netlistFile.substr(0, netlistFile.rfind(".bench"));
        string patternFile = pureName + ".pat";
        string reportFile = pureName + ".report";

        pat.open(patternFile, ios::out);
        report.open(reportFile, ios::out);
        setSPatternStream(pat.rdbuf());
        setReportStream(report.rdbuf());
        ParserTargetPath();

        faultFile = options.get<string>("fault");
        if (!faultFile.empty()) {
            faultMode = 'f';
        } else {
            faultFile = pureName + ".fault";
        }

        setWTestMode(1);
        setCctMode('9');
        setIseed(23);
    }

    const string &getExecAction() { return execAction; }
    const string &getFaultFileName() { return faultFile; }

    // cctMode
    char getCctMode(void) { return cctMode; }

    void setCctMode(char p_cctMode) {
        if (p_cctMode == ISCAS89 || p_cctMode == ISCAS85) {
            cctMode = p_cctMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be " << ISCAS85 << " or " << ISCAS89 << ".";
            throw ss.str();
        }
    };

    // randomLimit
    int getRandomLimit(void) { return randomLimit; };

    string GetPatternPath() const {
        //            return patternPath;
        char realp[PATH_MAX];
        realpath(patternPath.c_str(), realp);
        return string(realp);
    }

    string GetNetListPath() const {
        char realp[PATH_MAX];
        realpath(netlistFile.c_str(), realp);
        return string(realp);
    }
    void setRandomLimit(char p_randomLimit) {
        if (p_randomLimit >= 0 && p_randomLimit <= 32) {
            randomLimit = p_randomLimit;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be in range 0 to 32.";
            throw ss.str();
        }
    };

    // iseed
    unsigned int getIseed(void) { return iseed; };

    void setIseed(unsigned int p_iseed) { iseed = p_iseed; };

    // maxCompact
    int getMaxCompact(void) { return maxCompact; };

    void setMaxCompact(int p_maxCompact) { maxCompact = p_maxCompact; };

    // compact
    char getCompact(void) { return compact; };

    void setCompact(char p_compact) {
        if (p_compact == 's' || p_compact == 'n') {
            compact = p_compact;
            if (p_compact == 'n') maxCompact = 0;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 's' or 'n'.";
            throw ss.str();
        }
    };

    // maxBackTrack
    int getMaxBackTrack(void) { return maxBackTrack; };

    void setMaxBackTrack(int p_maxBackTrack) {
        if (p_maxBackTrack >= 1) {
            maxBackTrack = p_maxBackTrack;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be greater than 0.";
            throw ss.str();
        }
    };

    // maxBackTrack1
    int getMaxBackTrack1(void) { return maxBackTrack1; };

    void setMaxBackTrack1(int p_maxBackTrack1) {
        if (p_maxBackTrack1 >= 0) {
            maxBackTrack1 = p_maxBackTrack1;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be greater or equal to 0.";
            throw ss.str();
        }
    };

    // sPatternFile
    string getSPatternFile(void) { return sPatternFile; };

    void setSPatternFile(string p_sPatternFile) {
        sPatternFile = p_sPatternFile;
        sPatternStream = NULL;
    };

    // sPatternStream
    streambuf *getSPatternStream(void) { return sPatternStream; };

    void setSPatternStream(streambuf *p_sPatternStream) {
        sPatternStream = p_sPatternStream;
        sPatternFile = "";
    };

    // learnMode
    char getLearnMode(void) { return learnMode; };

    void setLearnMode(char p_learnMode) {
        if (p_learnMode == 'y' || p_learnMode == 'n') {
            learnMode = p_learnMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value has to be 'y' or 'n'.";
            throw ss.str();
        }
    };

    // faultMode
    char getFaultMode(void) { return faultMode; };

    void setFaultMode(char p_faultMode) {
        if (p_faultMode == 'f' || p_faultMode == 'd') {
            faultMode = p_faultMode;
            if (faultMode == 'd') faultFile = "";
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 'f' or 'd'.";
            throw ss.str();
        }
    };

    // faultFile
    string getFaultFile(void) {
        char realp[PATH_MAX] = {0};
        realpath(faultFile.c_str(), realp);
        return string(realp);
    };

    void setFaultFile(string p_faultFile) {
        faultFile = p_faultFile;
        faultMode = 'f';
    };

    // wFaultFile
    string getWFaultFile(void) { return wFaultFile; };

    void setWFaultFile(string p_wFaultFile) {
        wFaultFile = p_wFaultFile;
        wFaultStream = NULL;
    };

    // wFaultStream
    streambuf *getWFaultFileStream(void) { return wFaultStream; };

    void setWFaultStream(streambuf *p_wFaultStream) {
        wFaultStream = p_wFaultStream;
        wFaultFile = "";
    };

    // simMode
    char getSimMode(void) { return simMode; };

    void setSimMode(char p_simMode) {
        if (p_simMode == 'f' || p_simMode == 'h') {
            simMode = p_simMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 'f' or 'h'.";
            throw ss.str();
        }
    };

    // fillMode
    char getFillMode(void) { return fillMode; };

    void setFillMode(char p_fillMode) {
        if (p_fillMode == 'r' || p_fillMode == 'x' || p_fillMode == '0' || p_fillMode == '1') {
            fillMode = p_fillMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be '0' or '1' or 'r' or 'x'.";
            throw ss.str();
        }
    };

    // genAllPat
    char getGenAllPat(void) { return genAllPat; };

    void setGenAllPat(char p_genAllPat) {
        if (p_genAllPat == 'y' || p_genAllPat == 'n') {
            genAllPat = p_genAllPat;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 'y' or 'n'.";
            throw ss.str();
        }
    };

    // eachLimit
    int getEachLimit(void) { return eachLimit; };

    void setEachLimit(int p_eachLimit) { eachLimit = p_eachLimit; };

    // noFaultSim
    char getNoFaultSim(void) { return noFaultSim; };

    void setNoFaultSim(char p_noFaultSim) {
        if (p_noFaultSim == 'y' || p_noFaultSim == 'n') {
            noFaultSim = p_noFaultSim;
        } else {
            stringstream ss;
            ss << "Wrong value. Values must be 'y' or 'n'.";
            throw ss.str();
        }
    };

    // uFaultMode
    int getUFaultMode(void) { return uFaultMode; };

    void setUFaultMode(int p_uFaultMode) {
        if (p_uFaultMode >= 0 && p_uFaultMode <= 2) {
            uFaultMode = p_uFaultMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 0, 1 or 2.";
            throw ss.str();
        }
    };

    // udFaultsFile
    string getUdFaultsFile(void) { return udFaultsFile; };

    void setUdFaultsFile(string p_udFaultsFile) {
        udFaultsFile = p_udFaultsFile;
        udFaultsStream = NULL;
    };

    // udFaultsStream
    streambuf *getUdFaultsStream(void) { return udFaultsStream; };

    void setudFaultsStream(streambuf *p_udFaultsStream) {
        udFaultsStream = p_udFaultsStream;
        udFaultsFile = "";
    };

    // simulationMode
    int getSimulationMode(void) { return simulationMode; };

    void setSimulationMode(int p_simulationMode) {
        if (p_simulationMode == 0 || p_simulationMode == 1) {
            simulationMode = p_simulationMode;
            if (simulationMode) {
                simMode = 'h';
            }
        } else {
            stringstream ss;
            ss << "Wrong value. Values must be 0 or 1.";
            throw ss.str();
        }
    };

    // maskFile
    string getMaskFile(void) { return maskFile; };

    void setMaskFile(string p_maskFile) {
        maskFile = p_maskFile;
        maskStream = NULL;
    };

    // maskStream
    streambuf *getMaskStream(void) { return maskStream; };

    void setMaskStream(streambuf *p_maskStream) {
        maskStream = p_maskStream;
        maskFile = "";
    };

    // reportFile
    string getReportFile(void) { return reportFile; };

    void setReportFile(string p_reportFile) {
        reportFile = p_reportFile;
        reportStream = NULL;
    };

    // reportStream
    streambuf *getReportStream(void) { return reportStream; };

    void setReportStream(streambuf *p_reportStream) {
        reportStream = p_reportStream;
        reportFile = "";
    };

    // wTestMode
    int getWTestMode(void) { return wTestMode; };

    void setWTestMode(int p_wTestMode) {
        if (p_wTestMode >= 0 && p_wTestMode <= 4) {
            wTestMode = p_wTestMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be in range 0 to 4.";
            throw ss.str();
        }
    };

    // lfsrSimMode
    int getLfsrSimMode(void) { return lfsrSimMode; };

    void setLfsrSimMode(int p_lfsrSimMode) {
        if (p_lfsrSimMode >= 0 && p_lfsrSimMode <= 2) {
            lfsrSimMode = p_lfsrSimMode;
        } else {
            stringstream ss;
            ss << "Wrong value. Value must be 0, 1 or 2.";
            throw ss.str();
        }
    };

    // lfsrSimulation
    string getLfsrPoly(void) { return lfsrPoly; };

    string getLfsrSeed(void) { return lfsrSeed; };

    int getLfsrNum(void) { return lfsrNum; };

    void setLfsrSimulation(string p_lfsrPoly, string p_lfsrSeed, int p_lfsrNum) {
        lfsrPoly = p_lfsrPoly;
        lfsrSeed = p_lfsrSeed;
        lfsrNum = p_lfsrNum;
    };
};
}  // namespace hiatpg
#endif  //__ATALANTA_PARAMS_H__
