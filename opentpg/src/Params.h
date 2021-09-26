/**
 * Copyright (c) 2021 opentpg.com
 * opentpg is licensed under Mulan PSL v2.
 * You can use this software according to the terms and conditions of the Mulan PSL v2.
 * You may obtain a copy of Mulan PSL v2 at:
 *          http://license.coscl.org.cn/MulanPSL2
 * THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
 * EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
 * MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
 * See the Mulan PSL v2 for more details.
 */
#ifndef OPENTPG_PARAMS_H
#define OPENTPG_PARAMS_H

#include "cmdline.h"

class Params {
public:
    static Params& GetInstance();
    const std::string& GetNetlistFile() const {
        return netlistFile;
    }
    const std::string& GetFaultlistFile() const {
        return faultlistFile;
    }
    const std::string& GetConfigFile() const {
        return configFile;
    }
    const std::string& GetGateDumpFile() const {
        return gateDumpFile;
    }
    const std::string& GetCubeDumpFile() const {
        return cubeDumpFile;
    }
    bool EnableSimulate() const {
        return simulate;
    }
    void Load(int argc, char** argv);

private:
    Params();

private:
    std::string netlistFile;
    std::string faultlistFile;
    std::string configFile;
    std::string gateDumpFile;
    std::string cubeDumpFile;
    bool simulate { false };
    static Params instance;
};
#endif //OPENTPG_PARAMS_H
