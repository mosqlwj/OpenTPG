#ifndef __HIPARSER_PARAMS_H__
#define __HIPARSER_PARAMS_H__

#include "cmdline.h"

class Params {
public:
    static Params& GetInstance();
    const std::string& GetNetlistFile() const
    {
        return netlistFile;
    }
    const std::string& GetFaultlistFile() const
    {
        return faultlistFile;
    }
    const std::string& GetConfigFile() const
    {
        return configFile;
    }
    const std::string& GetGateDumpFile() const
    {
        return gateDumpFile;
    }
    const std::string& GetCubeDumpFile() const
    {
        return cubeDumpFile;
    }
    void parseCheck(int argc, char* argv[]);

private:
    Params();

private:
    std::string netlistFile;
    std::string faultlistFile;
    std::string configFile;
    std::string gateDumpFile;
    std::string cubeDumpFile;
    static Params instance;
};
#endif
