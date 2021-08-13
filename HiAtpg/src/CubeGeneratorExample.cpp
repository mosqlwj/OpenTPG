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

FaultStatus CubeGeneratorExample::Generate(const Fault* fault, TestCube*& cube)
{
    cube = TestCube::GenRandomCube(context->netlist, fault->id);
    if (cube == nullptr) {
        return FaultStatus::UNTESTABLE;
    }

    return FaultStatus::TESTED;
}
