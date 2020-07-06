// atalanta.cpp : Defines the entry point for the console application.
//
#include "Atpg.h"
#include "Simulation.h"

#include <stdio.h>
#include <vector>

using namespace atalantadll;

int main(int argc, char** argv)
{
	int res = 0;
	fstream bench, pat;

    Atalanta* atlanta =new Atalanta();
    Params* paras;

    paras = new Params();
    bench.open("c17.bench", ios::in);
    pat.open("c17.pat", ios::out);
    paras->setBenchStream(bench.rdbuf());
    paras->setSPatternStream(pat.rdbuf());
    paras->setWTestMode(1);
    paras->setCctMode('9');
    paras->setIseed(23);
    atlanta->setParams(paras);
    atlanta->run();

	return res;	
}



