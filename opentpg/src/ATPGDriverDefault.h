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
#ifndef R7_ATPGDRIVERDEFAULT_H
#define R7_ATPGDRIVERDEFAULT_H

#include "ATPGDriver.h"

#include <string>

class Netlist;
class Faultlist;
class CubeHandler;
class ContextDefault;
class CubeGenerator;

class ATPGDriverDefault : public ATPGDriver {
public:
    explicit ATPGDriverDefault(ContextDefault* context);
    ~ATPGDriverDefault() override;
    virtual void SetupNetlist(Netlist* n) override;
    virtual void SetupFaultlist(Faultlist* f) override;
    virtual void SetupCubeOutput(CubeHandler* cubeOutput) override;
    virtual int Prepare() override;
    virtual void Execute() override;
    virtual void Cleanup() override;

private:
    ContextDefault* context;
    Netlist* netlist;
    Faultlist* faultlist;
    CubeHandler* cubeHandler;
    CubeGenerator* cubeGenerator;
};

#endif //R7_ATPGDRIVERDEFAULT_H
