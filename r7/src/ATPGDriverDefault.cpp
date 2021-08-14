/**
 * Copyright (c) [Year] [name of copyright holder]
 * [Software Name] is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#include "ATPGDriverDefault.h"

#include "ContextDefault.h"
#include "CubeGenerator.h"
#include "CubeHandler.h"
#include "Faultlist.h"
#include "Netlist.h"
#include "asserts.h"

ATPGDriverDefault::ATPGDriverDefault(ContextDefault* context)
    : context(context)
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
    ASSERT(cubeHandler == nullptr);
    cubeHandler = c;
}

int ATPGDriverDefault::Prepare()
{
    cubeGenerator = CreateCubeGenerator(context);
    ASSERT(cubeGenerator != nullptr);

    return 0;
}

void ATPGDriverDefault::Execute()
{
    ASSERT(faultlist != nullptr);
    ASSERT(cubeHandler != nullptr);
    ASSERT(cubeGenerator != nullptr);

    const std::vector<Fault*>& flist = faultlist->Faults();
    for (std::size_t i = 0; i < flist.size(); i++) {
        //  获得一个 fault
        Fault* fault = flist[i];
        ASSERT(fault != nullptr);

        //  对每个 fault 尝试生成一个 cube
        TestCube* cube = nullptr;
        FaultStatus faultStatus = cubeGenerator->Generate(fault, cube);

        //  标记 fault 的检测状态
        fault->status = faultStatus;

        //  如果能够生成 cube，那么触发 Handle 处理
        if (cube != nullptr) {
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
