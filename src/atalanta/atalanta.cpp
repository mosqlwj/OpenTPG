// atalanta.cpp : Defines the entry point for the console application.
//
#include "Atpg.h"
#include "Simulation.h"

#include <stdio.h>
#include <vector>

using namespace hiatpg;

int main(int argc, char** argv)
{
    auto params = &Params::getInstance();
    params->parseCheck(argc, argv);

    Atalanta* atlanta =new Atalanta();
    atlanta->setParams();
    atlanta->run();

    return 0;
}



