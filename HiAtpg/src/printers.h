#ifndef HIATPG_CUBEOUTPUT_H
#define HIATPG_CUBEOUTPUT_H

#include "Fault.h"
#include "TestCube.h"

#include <fstream>

extern std::ofstream& operator<<(std::ofstream& stream, const TestCube& cube);

extern std::ofstream& operator<<(std::ofstream& stream, const Fault& fault);
#endif //HIATPG_CUBEOUTPUT_H
