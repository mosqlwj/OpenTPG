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
#include "CubeGeneratorExample.h"

#include "ContextDefault.h"
#include "Netlist.h"
#include "TestCube.h"
#include "asserts.h"

CubeGeneratorExample::CubeGeneratorExample(ContextDefault* context)
    : context(context)
{
    ASSERT(context != nullptr);
    ASSERT(context->netlist != nullptr);
}

CubeGeneratorExample::~CubeGeneratorExample()
{
}

FaultStatus CubeGeneratorExample::Generate(const Fault* fault, TestCube*& cube)
{
    ASSERT(fault != nullptr);
    ASSERT(cube == nullptr);
    ASSERT(context != nullptr);
    ASSERT(context->netlist != nullptr);

    const Netlist* netlist = context->netlist;

    //  构造一个 cube 对象
    cube = new TestCube();

    //  随机生成 cycle 数
    int cycleNum = rand() % 3 + 1;

    //  先初始化下 cube
    int ret = cube->Init(netlist->GetPICount(), netlist->GetScanCellCount(), cycleNum, fault->id);
    if (ret != 0) {
        delete cube;
        cube = nullptr;
        return FaultStatus::UNTESTABLE;
    }

    //  纯随机填充算法
    for (size_t cycleId = 0; cycleId < cycleNum; cycleId++) {
        int bitSize = (cycleId != 0) ? netlist->GetPICount() : netlist->GetPICount() + netlist->GetScanCellCount();
        for (size_t i = 0; i < bitSize; i++) {
            int randBit = rand() % (LogicVal::LOGIC_COUNT - 1);
            cube->UpdateTestCubeValue(cycleId, i, LogicVal(randBit));
        }
    }

    return FaultStatus::TESTED;
}
