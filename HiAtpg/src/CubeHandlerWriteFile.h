//
// Created by luolijun on 2021/8/12.
//

#ifndef CUBEHANDLERWRITEFILE_H
#define CUBEHANDLERWRITEFILE_H

#include "CubeHandler.h"
#include "TestCube.h"
#include "asserts.h"
#include "printers.h"

#include <fstream>
#include <string>

class CubeHandlerWriteFile : public CubeHandler {

public:
    CubeHandlerWriteFile(const std::string& cubefile)
    {
        stream.open(cubefile);
        if (!stream.is_open()) {
            printf("open  %s failed.\n", cubefile.c_str());
            return;
        }
    }

    ~CubeHandlerWriteFile() override
    {
    }

public:
    void Handle(TestCube* cube) override
    {
        ASSERT(stream != nullptr);
        ASSERT(stream.is_open());
        stream << (*cube);
    }

private:
    std::ofstream stream;
};

#endif //CUBEHANDLERWRITEFILE_H
