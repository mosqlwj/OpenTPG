#include "AtpgEngine.h"
#include "PatternParser.h"
using namespace hiatpg;

int main(int argc, char** argv)
{
    auto params = &Params::getInstance();
    params->parseCheck(argc, argv);

    if (params->getExecAction() == "origin") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->run();
    } else if (params->getExecAction() == "atpg") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->generateCube();
    } else if (params->getExecAction() == "rp"){
        PatternParser* patternEngine = new PatternParser();
        patternEngine->setParams();
        patternEngine->run();
    } else if (params->getExecAction() == "stat"){
        PatternParser* parserEngine = new PatternParser();
        parserEngine->setParams();
        parserEngine->ReadPattern();
        parserEngine->ReadFault();
        parserEngine->PrintLog(*params);
    } else{
        cerr << "unsupported work mode" << endl;
    }

    return 0;
}



