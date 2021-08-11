//
// Created by fish on 2021/7/2.
//

#include "Params.h"

Params* Params::instance = nullptr;

Params*
Params::GetInstance()
{
  if (instance == nullptr) {
    instance = new Params();
  }

  return instance;
}

Params::Params() {}

void
Params::parseCheck(int argc, char** argv)
{
  cmdline::parser options;
  // 加入指定类型的输入參数
  // 第一个參数：长名称
  // 第二个參数：短名称（‘\0‘表示没有短名称）
  // 第三个參数：參数描写叙述
  // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
  // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
  options.add<std::string>("netlist",                                   //
                           'n',                                         //
                           "The netlist file name such as ./s27.bench", //
                           true);
  options.add<std::string>("fault",                                      //
                           'f',                                          //
                           "Create fault list file such as ./s27.fault", //
                           false,                                        //
                           "");
  options.add<std::string>("config",                             //
                           'c',                                  //
                           "read config file such as ./s27.cfg", //
                           true);
  options.add<std::string>("gate",                              //
                           'g',                                 //
                           "dump gate file such as ./s27.gate", //
                           false,                               //
                           "");
  options.add<std::string>(
    "cube",                                                        //
    'u',                                                           //
    "generate the cubes and write to the spec such as ./s27.cube", //
    false,                                                         //
    "");
  options.parse_check(argc, argv);
  netlistFile = options.get<std::string>("netlist");
  faultlistFile = options.get<std::string>("fault");
  configFile = options.get<std::string>("config");
  gateDumpFile = options.get<std::string>("gate");
  cubeDumpFile = options.get<std::string>("cube");
}
