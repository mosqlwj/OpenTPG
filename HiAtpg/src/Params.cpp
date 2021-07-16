//
// Created by fish on 2021/7/2.
//

#include "Params.h"

Params* Params::instance = nullptr;

Params *Params::GetInstance() {
    if (instance == nullptr) {
        instance = new Params();
    }
    return instance;
}

Params::Params() {

}
