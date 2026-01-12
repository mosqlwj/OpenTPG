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

#include "flowtpg/model/Fault.h"

class FaultTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        testGate = new Gate("test_gate", GateType::AND);
    }

    void TearDown() override
    {
        delete testGate;
    }

    Gate* testGate;
};

TEST_F(FaultTest, CreateStuckAt0Fault)
{
    Fault fault(FaultType::STUCK_AT_0, testGate, 0);

    EXPECT_EQ(fault.type, FaultType::STUCK_AT_0);
    EXPECT_EQ(fault.gate, testGate);
    EXPECT_EQ(fault.pin, 0);
    EXPECT_EQ(fault.id, -1);
}

TEST_F(FaultTest, CreateStuckAt1Fault)
{
    Fault fault(FaultType::STUCK_AT_1, testGate, 1);

    EXPECT_EQ(fault.type, FaultType::STUCK_AT_1);
    EXPECT_EQ(fault.gate, testGate);
    EXPECT_EQ(fault.pin, 1);
}

TEST_F(FaultTest, FaultTypes)
{
    Fault sa0(FaultType::STUCK_AT_0, testGate, 0);
    Fault sa1(FaultType::STUCK_AT_1, testGate, 0);

    EXPECT_EQ(sa0.type, FaultType::STUCK_AT_0);
    EXPECT_EQ(sa1.type, FaultType::STUCK_AT_1);

    EXPECT_NE(sa0.type, sa1.type);
}
