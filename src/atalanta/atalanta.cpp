// atalanta.cpp : Defines the entry point for the console application.
//
#include "AtpgEngine.h"
#include "Simulation.h"

#include <stdio.h>
#include <vector>

using namespace hiatpg;

int main(int argc, char** argv)
{
    auto params = &Params::getInstance();
    params->parseCheck(argc, argv);

    if (params->getExecAction() == "atpg") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->run();
    } else if (params->getExecAction() == "cube") {
        AtpgEngine* atpgEngine =new AtpgEngine();
        atpgEngine->setParams();
        atpgEngine->run();
    } else {
        cerr << "unsupported work mode" << endl;
    }

    return 0;
}



