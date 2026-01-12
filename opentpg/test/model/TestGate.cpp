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

#include "flowtpg/model/Gate.h"

class GateTest : public ::testing::Test
{
protected:
    void SetUp() override
    {}

    void TearDown() override
    {}
};

TEST_F(GateTest, CreatePIGate)
{
    Gate pi("input1", GateType::PI);
    EXPECT_EQ(pi.name, "input1");
    EXPECT_EQ(pi.type, GateType::PI);
    EXPECT_EQ(pi.inputs.size(), 0);
    EXPECT_EQ(pi.outputs.size(), 0);
}

TEST_F(GateTest, CreateANDGate)
{
    Gate andGate("and1", GateType::AND);
    EXPECT_EQ(andGate.name, "and1");
    EXPECT_EQ(andGate.type, GateType::AND);
}

TEST_F(GateTest, GateTypeToString)
{
    EXPECT_STREQ(StringOf(GateType::PI), "INPUT");
    EXPECT_STREQ(StringOf(GateType::DFF), "DFF");
    EXPECT_STREQ(StringOf(GateType::AND), "AND");
    EXPECT_STREQ(StringOf(GateType::NAND), "NAND");
    EXPECT_STREQ(StringOf(GateType::OR), "OR");
    EXPECT_STREQ(StringOf(GateType::NOR), "NOR");
    EXPECT_STREQ(StringOf(GateType::XOR), "XOR");
    EXPECT_STREQ(StringOf(GateType::XNOR), "XNOR");
    EXPECT_STREQ(StringOf(GateType::INV), "NOT");
    EXPECT_STREQ(StringOf(GateType::BUF), "BUF");
    EXPECT_STREQ(StringOf(GateType::PO), "OUTPUT");
}

TEST_F(GateTest, GateInputsOutputs)
{
    Gate input1("in1", GateType::PI);
    Gate input2("in2", GateType::PI);
    Gate andGate("and1", GateType::AND);
    Gate output("out1", GateType::PO);

    andGate.inputs.push_back(&input1);
    andGate.inputs.push_back(&input2);
    andGate.outputs.push_back(&output);

    EXPECT_EQ(andGate.inputs.size(), 2);
    EXPECT_EQ(andGate.outputs.size(), 1);
    EXPECT_EQ(andGate.inputs[0]->name, "in1");
    EXPECT_EQ(andGate.inputs[1]->name, "in2");
    EXPECT_EQ(andGate.outputs[0]->name, "out1");
}
