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
#ifndef OPENTPG_SIMENTRYDEFAULT_H
#define OPENTPG_SIMENTRYDEFAULT_H

#include "Faultlist.h"
#include "SimFault.h"
#include "SimGood.h"
#include "Simulator.h"
#include "TestCube.h"

class SimulatorDefault : public Simulator {
public:
    SimulatorDefault()
    {
        goodSimulator = new SimGood();
        faultSimulator = new SimFault();
    }

    void SetupNetlist(Netlist* netlist)
    {
        goodSimulator->SetupNetlist(netlist);
        faultSimulator->SetupNetlist(netlist);
    }

    void SetupFaultlist(Faultlist* flist)
    {
        faultSimulator->SetupFaultlist(flist);
    }

    ~SimulatorDefault() override
    {
        delete goodSimulator;
        delete faultSimulator;
    }

    bool HandleTestCube(TestCube* testCube) override;

private:
    SimGood* goodSimulator { nullptr };
    SimFault* faultSimulator { nullptr };
};

#endif //OPENTPG_SIMENTRYDEFAULT_H
