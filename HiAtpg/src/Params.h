#ifndef __HIPARSER_PARAMS_H__
#define __HIPARSER_PARAMS_H__

#include "cmdline.h"

class Params {
private:
    Params();
    std::string netlistFile;
    std::string faultlistFile;
    std::string configFile;
    std::string gateDumpFile;
    static Params* instance;

public:
    static Params* GetInstance();
    const std::string& GetNetlistFile() const { return netlistFile; }
    const std::string& GetFaultlistFile() const { return faultlistFile; }
    const std::string& GetConfigFile() const { return configFile; }
    const std::string& GetGateDumpFile() const { return gateDumpFile; }
    void parseCheck(int argc, char *argv[]);
};
#endif
