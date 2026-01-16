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
#include "context/ATPGContext.h"

namespace opentpg {
namespace core {
namespace context {

ATPGContext::ATPGContext()
    : threadPool_(std::make_unique<thread::ThreadPool>())
    , parallelEngine_(std::make_unique<thread::ParallelEngine>(*threadPool_))
{
}

void ATPGContext::SetNetlist(std::shared_ptr<model::netlist::Netlist> netlist)
{
    netlist_ = netlist;
}

void ATPGContext::SetFaultList(std::shared_ptr<model::fault::FaultList> faultList)
{
    faultList_ = faultList;
}

void ATPGContext::SetSimulationContext(std::shared_ptr<simulation::SimulationContext> simContext)
{
    simContext_ = simContext;
}

void ATPGContext::SetConfig(const std::string& key, const std::string& value)
{
    configs_[key] = value;
}

void ATPGContext::SetConfigInt(const std::string& key, int value)
{
    configsInt_[key] = value;
}

std::string ATPGContext::GetConfig(const std::string& key, const std::string& defaultValue) const
{
    auto it = configs_.find(key);
    return it != configs_.end() ? it->second : defaultValue;
}

int ATPGContext::GetConfigInt(const std::string& key, int defaultValue) const
{
    auto it = configsInt_.find(key);
    return it != configsInt_.end() ? it->second : defaultValue;
}

void ATPGContext::Reset()
{
    netlist_.reset();
    faultList_.reset();
    simContext_.reset();
    configs_.clear();
    configsInt_.clear();
}

} // namespace context
} // namespace core
} // namespace opentpg
