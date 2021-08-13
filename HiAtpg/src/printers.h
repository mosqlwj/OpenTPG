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
#ifndef HIATPG_CUBEOUTPUT_H
#define HIATPG_CUBEOUTPUT_H

#include "Fault.h"
#include "TestCube.h"

#include <fstream>

extern std::ofstream& operator<<(std::ofstream& stream, const TestCube& cube);

extern std::ofstream& operator<<(std::ofstream& stream, const Fault& fault);
#endif //HIATPG_CUBEOUTPUT_H
