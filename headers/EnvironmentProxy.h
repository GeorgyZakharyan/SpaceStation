#pragma once
#include <map>
#include <memory>
#include <string>
#include "Environment.h"

class EnvironmentProxy : public Environment
{
private:
    std::shared_ptr<Environment> realEnvironment_;
    std::map<int, std::string> cache_;      

public:
    explicit EnvironmentProxy(const std::shared_ptr<Environment>& realEnvironment);

    void applyEffect(Module& module) override;
    std::string getName() const override;
    double getEnergyModifier() const override;
    double getDamageModifier() const override;

    std::shared_ptr<Environment> getRealEnvironment() const;
    void clearCache();                      
    int getCacheSize() const;
};
