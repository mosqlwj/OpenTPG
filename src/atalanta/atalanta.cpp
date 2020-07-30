#include "AtpgEngine.h"
#include "PatternParser.h"
using namespace hiatpg;

int main(int argc, char** argv)
{
    auto params = &Params::getInstance();
    params->parseCheck(argc, argv);

    string action = params->getExecAction();
    if (action == "origin") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->generatePattern();
    } else if (action == "atpg") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->generateCube();
    } else if (action == "simulate-pattern") {
        PatternParser* patternEngine = new PatternParser();
        patternEngine->setParams();
        patternEngine->run("pattern");
    } else if (action == "simulate-cube") {
        PatternParser* patternEngine = new PatternParser();
        patternEngine->setParams();
        patternEngine->run("cube");
    } else if (action == "stat") {
        PatternParser* parserEngine = new PatternParser();
        parserEngine->setParams();
        parserEngine->ReadPattern();
        parserEngine->ReadFault();
        parserEngine->PrintLog(*params);
    } else if (action == "create-fault") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->createFault();
    } else if (action == "upload-netlist") {

    } else{
        std::cerr << "Unsupported action, type -h for help: '" << action << "'" << std::endl;
    }

    return 0;
}



