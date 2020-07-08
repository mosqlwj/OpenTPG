// atalanta.cpp : Defines the entry point for the console application.
//
#include "Atpg.h"
#include "Simulation.h"

#include <stdio.h>
#include <vector>
#include "cmdline.h"

using namespace hiatpg;

int main(int argc, char** argv)
{
    cmdline::parser options;
    // 加入指定类型的输入參数
    // 第一个參数：长名称
    // 第二个參数：短名称（‘\0‘表示没有短名称）
    // 第三个參数：參数描写叙述
    // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
    // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
    options.add<string>("bench", 'b', "bench file name", true, "");
//    options.add<string>("fault", 'f', "fault file name", true, "");
    options.parse_check(argc, argv);

    string benchFile = options.get<string>("bench");

    int res = 0;
    fstream bench, pat;

    Atalanta* atlanta =new Atalanta();
    Params* paras;

    paras = new Params();
    bench.open(benchFile, ios::in);
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



