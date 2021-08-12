#ifndef HIATPG_CUBEOUTPUT_H
#define HIATPG_CUBEOUTPUT_H

#include "TestCube.h"

#include <fstream>

extern std::ofstream& operator<<(std::ofstream& stream, const TestCube& cube);

#endif //HIATPG_CUBEOUTPUT_H
