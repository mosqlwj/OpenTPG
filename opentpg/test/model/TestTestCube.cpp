/**
 * Copyright (c) 2025 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */

#include <gtest/gtest.h>

#include "flowtpg/model/TestCube.h"

class TestCubeTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        cube = new TestCube();
    }

    void TearDown() override
    {
        delete cube;
    }

    TestCube* cube;
};

TEST_F(TestCubeTest, CreateTestCube)
{
    ASSERT_NE(cube, nullptr);
}

TEST_F(TestCubeTest, InitTestCube)
{
    int result = cube->Init(10, 5, 3, 100);
    EXPECT_EQ(result, 0);
    EXPECT_EQ(cube->GetCycleSize(), 3);
    EXPECT_EQ(cube->GetFaultIndex(), 100);
}

TEST_F(TestCubeTest, InitWithZeroCycle)
{
    int result = cube->Init(10, 5, 0, 100);
    EXPECT_EQ(result, -1);
}

TEST_F(TestCubeTest, InitWithNegativeCycle)
{
    int result = cube->Init(10, 5, -1, 100);
    EXPECT_EQ(result, -1);
}

TEST_F(TestCubeTest, UpdateTestCubeValue)
{
    cube->Init(10, 5, 2, 100);

    cube->UpdateTestCubeValue(0, 0, LogicVal::LOGIC_0);
    cube->UpdateTestCubeValue(0, 1, LogicVal::LOGIC_1);
    cube->UpdateTestCubeValue(1, 5, LogicVal::LOGIC_X);

    const auto& logicValue = cube->GetLogicValue();
    EXPECT_EQ(logicValue[0][0], LogicVal::LOGIC_0);
    EXPECT_EQ(logicValue[0][1], LogicVal::LOGIC_1);
    EXPECT_EQ(logicValue[1][5], LogicVal::LOGIC_X);
}

TEST_F(TestCubeTest, GetLogicValue)
{
    cube->Init(5, 3, 1, 50);

    const auto& logicValue = cube->GetLogicValue();
    EXPECT_EQ(logicValue.size(), 1);
    EXPECT_EQ(logicValue[0].size(), 8);
}

TEST_F(TestCubeTest, GetFaultIndex)
{
    cube->Init(5, 3, 1, 42);
    EXPECT_EQ(cube->GetFaultIndex(), 42);
}
