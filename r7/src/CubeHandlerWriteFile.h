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
#ifndef R7_CUBEHANDLERWRITEFILE_H
#define R7_CUBEHANDLERWRITEFILE_H

#include "CubeHandler.h"
#include "TestCube.h"
#include "asserts.h"
#include "printers.h"

#include <fstream>
#include <string>

class CubeHandlerWriteFile : public CubeHandler {

public:
    explicit CubeHandlerWriteFile(const std::string& cubefile)
    {
        stream.open(cubefile);
        if (!stream.is_open()) {
            std::cerr << "open  " << cubefile << " failed." << std::endl;
            return;
        }
    }

    ~CubeHandlerWriteFile() override = default;

public:
    void Handle(TestCube* cube) override
    {
        ASSERT(cube != nullptr);
        ASSERT(stream.is_open());
        stream << (*cube);
    }

private:
    std::ofstream stream;
};

#endif //R7_CUBEHANDLERWRITEFILE_H
