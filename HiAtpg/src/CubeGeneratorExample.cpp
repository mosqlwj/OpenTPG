#include "CubeGeneratorExample.h"

#include "ContextDefault.h"
#include "Netlist.h"
#include "TestCube.h"
#include "asserts.h"

CubeGeneratorExample::CubeGeneratorExample(ContextDefault* context)
    : context(context)
{
    ASSERT(context != nullptr);
    ASSERT(context->netlist != nullptr);
}

CubeGeneratorExample::~CubeGeneratorExample()
{
}

TestCube* CubeGeneratorExample::Generate(const Fault* fault)
{
    return TestCube::GenRandomCube(context->netlist, fault->id);
}
