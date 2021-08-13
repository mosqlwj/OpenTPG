#ifndef ATPGDRIVERDEFAULT_H
#define ATPGDRIVERDEFAULT_H

#include "ATPGDriver.h"

#include <string>

class Netlist;
class Faultlist;
class CubeHandler;
class ContextDefault;
class CubeGenerator;

class ATPGDriverDefault : public ATPGDriver {
public:
    ATPGDriverDefault(ContextDefault* context);
    ~ATPGDriverDefault() override;
    void SetupNetlist(Netlist* n) override;
    void SetupFaultlist(Faultlist* f) override;
    void SetupCubeOutput(CubeHandler* cubeOutput) override;
    int Prepare() override;
    void Execute() override;
    void Cleanup() override;

private:
    ContextDefault* context;
    Netlist* netlist;
    Faultlist* faultlist;
    CubeHandler* cubeHandler;
    CubeGenerator* cubeGenerator;
};

#endif // ATPGDRIVERDEFAULT_H
