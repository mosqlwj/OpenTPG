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
#ifndef R7_CUBEGENERATOR_H
#define R7_CUBEGENERATOR_H

#include "common.h"

struct Fault;
struct TestCube;

class CubeGenerator {
public:
    virtual ~CubeGenerator() = default;
    virtual FaultStatus Generate(const Fault* fault, TestCube*& cube) = 0;
};

extern CubeGenerator* CreateCubeGenerator(void* context);

#endif //R7_CUBEGENERATOR_H
