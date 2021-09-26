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
#ifndef OPENTPG_CUBEHANDLERWRITEFILE_H
#define OPENTPG_CUBEHANDLERWRITEFILE_H

#include "CubeHandler.h"
#include "Simulator.h"
#include "TestCube.h"
#include "asserts.h"
#include "printers.h"

#include <fstream>
#include <iostream>
#include <string>

class CubeHandlerWriteFile : public CubeHandler {

public:
    CubeHandlerWriteFile(const std::string& cubefile, Simulator* sim = nullptr)
    {
        stream.open(cubefile);
        if (!stream.is_open()) {
            std::cerr << "open  " << cubefile << " failed." << std::endl;
            return;
        }

        simulator = sim;
    }

    ~CubeHandlerWriteFile() override = default;

public:
    void Handle(TestCube* cube) override
    {
        ASSERT(cube != nullptr);
        ASSERT(stream.is_open());
        stream << (*cube);

        if (simulator != nullptr) {
            bool detected = simulator->HandleTestCube(cube);
            std::cout << "CUBE " << cube->GetFaultIndex() << " " << (detected ? "DETECTED" : "UNDETECTED") << std::endl;
        }
    }

private:
    std::ofstream stream;
    Simulator* simulator { nullptr };
};

#endif //OPENTPG_CUBEHANDLERWRITEFILE_H
