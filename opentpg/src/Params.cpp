/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "Params.h"

Params Params::instance;

Params& Params::GetInstance() {
    return instance;
}

Params::Params() {
}

void Params::Load(int argc, char** argv) {
    cmdline::parser options;
    // 加入指定类型的输入參数
    // 第一个參数：长名称
    // 第二个參数：短名称（‘\0‘表示没有短名称）
    // 第三个參数：參数描写叙述
    // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
    // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
    options.add<std::string>("netlist",              //
        'n',                                         //
        "The netlist file name such as ./s27.bench", //
        true);
    options.add<std::string>("fault",                 //
        'f',                                          //
        "Create fault list file such as ./s27.fault", //
        true,                                         //
        "");
    options.add<std::string>("config",        //
        'c',                                  //
        "Read config file such as ./s27.cfg", //
        true);
    options.add<std::string>("gate",         //
        'g',                                 //
        "Dump gate file such as ./s27.gate", //
        false,                               //
        "");
    options.add<std::string>(
        "cube",                                                        //
        'u',                                                           //
        "Generate the cubes and write to the spec such as ./s27.cube", //
        true,                                                          //
        "");
    options.parse_check(argc, argv);
    netlistFile = options.get<std::string>("netlist");
    faultlistFile = options.get<std::string>("fault");
    configFile = options.get<std::string>("config");
    gateDumpFile = options.get<std::string>("gate");
    cubeDumpFile = options.get<std::string>("cube");
}
