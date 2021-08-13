#include "ATPGDriverDefault.h"

#include "CubeGenerator.h"
#include "CubeHandler.h"
#include "Netlist.h"
#include "asserts.h"

ATPGDriverDefault::ATPGDriverDefault()
{
    netlist = nullptr;
    faultlist = nullptr;
    cubeHandler = nullptr;
    cubeGenerator = nullptr;
}

ATPGDriverDefault::~ATPGDriverDefault()
{
}

void ATPGDriverDefault::SetupNetlist(Netlist* n)
{
    ASSERT(n != nullptr);
    ASSERT(netlist == nullptr);
    netlist = n;
}

void ATPGDriverDefault::SetupFaultlist(Faultlist* f)
{
    ASSERT(f != nullptr);
    ASSERT(faultlist == nullptr);
    faultlist = f;
}

void ATPGDriverDefault::SetupCubeOutput(CubeHandler* c)
{
    ASSERT(c != nullptr);
    ASSERT(cubeOutput == nullptr);
    cubeHandler = c;
}

int ATPGDriverDefault::Prepare()
{
    cubeGenerator = CreateCubeGenerator();
    ASSERT(cubeGenerator != nullptr);

    return 0;
}

void ATPGDriverDefault::Execute()
{
    ASSERT(faultlist != nullptr);
    ASSERT(cubeOutput != nullptr);
    ASSERT(cubeGenerator != nullptr);

    const std::vector<Fault*>& flist = netlist->GetFaultList();
    for (std::size_t i = 0; i < flist.size(); i++) {
        //  获得一个 fault
        Fault* fault = flist[i];
        ASSERT(fault != nullptr);

        //  对每个 fault 尝试生成一个 cube
        TestCube* cube = nullptr;
        FaultStatus faultStatus = cubeGenerator->Generate(fault, cube);
        fault->status = faultStatus;

        //  一旦生成了 cube，交个 cubeHandler 处理
        if (faultStatus == TESTED) {
            cubeHandler->Handle(cube);
        }
    }
}

void ATPGDriverDefault::Cleanup()
{
    netlist = nullptr;
    faultlist = nullptr;
    cubeHandler = nullptr;
    cubeGenerator = nullptr;
}
