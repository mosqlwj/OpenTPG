//
// Created by luolijun on 2021/8/13.
//

#ifndef ERRORS_H
#define ERRORS_H

#define ERRID_TABLE()                     \
    DEF_ERRID(1, LOAD_NETLIST_FAILED)     \
    DEF_ERRID(2, CREATE_FAULTLIST_FAILED) \
    DEF_ERRID(3, ATPG_PREPARE_FAILED)     \
    /* (end) */

namespace errors {

enum : int {
#define DEF_ERRID(id, name) name = id,
    ERRID_TABLE()
#undef DEF_ERRID
};

}; // errors

#endif //ERRORS_H
