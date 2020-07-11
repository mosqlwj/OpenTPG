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

    AtpgEngine* atpgEngine =new AtpgEngine();
    atpgEngine->setParams();
    atpgEngine->run();

    return 0;
}



