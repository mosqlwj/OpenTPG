#ifndef __HIPARSER_PARAMS_H__
#define __HIPARSER_PARAMS_H__

#include "cmdline.h"

class Params {
private:
    Params();
    std::string netlistFile;
    std::string faultlistFile;
    static Params* instance;

public:
    static Params* GetInstance();
    const std::string& GetNetlistFile() const { return netlistFile; }
    const std::string& GetFaultlistFile() const { return faultlistFile; }

    void parseCheck(int argc, char *argv[]) {
        cmdline::parser options;
        // 加入指定类型的输入參数
        // 第一个參数：长名称
        // 第二个參数：短名称（‘\0‘表示没有短名称）
        // 第三个參数：參数描写叙述
        // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
        // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
        options.add<std::string>("netlist",
                                 'n',
                                 "The netlist file name such as ./c17.bench",
                                 true);
        options.add<std::string>("create-fault",
                                 'c',
                                 "Create fault list file such as ./c17.fault",
                                 false,
                                 "");
        options.parse_check(argc, argv);
        netlistFile = options.get<std::string>("netlist");
        faultlistFile = options.get<std::string>("create-fault");
    }
};
#endif
