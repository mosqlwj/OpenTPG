TEMPLATE  =   app
TARGET    =   hiatpg
CONFIG    -=  qt5
CONFIG    +=  console

include($$PWD/hiatpg.pri)
include($$PWD/atpg/atpg.pri)
include($$PWD/fault/fault.pri)
include($$PWD/include/include.pri)
include($$PWD/netlist/netlist.pri)
include($$PWD/pattern/pattern.pri)
include($$PWD/sim/sim.pri)
include($$PWD/utils/utils.pri)

