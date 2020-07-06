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
	
	try {
		Atalanta *simulation=new Atalanta();
		Params *p[5];

		//res=simulation->run(argc, argv);

		p[0] = new Params();
		bench.open("c17.bench", ios::in);
		pat.open("c17.pat", ios::out);
		p[0]->setBenchStream(bench.rdbuf());
		p[0]->setSPatternStream(pat.rdbuf());
		p[0]->setWTestMode(1);
		p[0]->setCctMode('9');
		p[0]->setIseed(23);
		simulation->setParams(p[0]);
		simulation->run();
	}
	catch(string s) {
		cerr << s;
	}
	catch(...) {
		cerr << "Uknown exception";
	}
	return res;	
}



