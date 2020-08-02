#include "cmdline.h"
#include "model.h"

int main(int argc, char* argv[])
{
    cmdline::parser options;
    // 加入指定类型的输入參数
    // 第一个參数：长名称
    // 第二个參数：短名称（‘\0‘表示没有短名称）
    // 第三个參数：參数描写叙述
    // 第四个參数：bool值，表示该參数是否必须存在（可选。默认值是false）
    // 第五个參数：參数的默认值（可选，当第四个參数为false时该參数有效）
    options.add<std::string>("bench", 'b', "bench file name", true, "");
    options.add<std::string>("fault", 'f', "fault file name", true, "");
    options.parse_check(argc, argv);

    auto netlist = NetList::GetInstance();
    string benchFile = options.get<string>("bench");
    netlist->Parse(benchFile);
    netlist->PrintGates();

    auto faultlist = FaultList::GetInstance();
    string faultFile = options.get<string>("fault");;
    faultlist->Parse(faultFile);
    faultlist->PrintFaults();

    return 0;
}
