#pragma once
#include <memory>
#include <string>
#include "Module.h"

class ModuleFactory
{
public:
    virtual ~ModuleFactory();

    virtual std::unique_ptr<Module> createResidentialModule(const std::string& name, int variant = 0) = 0;
    virtual std::unique_ptr<Module> createEnergyModule(const std::string& name, int variant = 0) = 0;
    virtual std::unique_ptr<Module> createResearchModule(const std::string& name, int variant = 0) = 0;
};

class StandardModuleFactory : public ModuleFactory
{
public:
    std::unique_ptr<Module> createResidentialModule(const std::string& name, int variant = 0) override;
    std::unique_ptr<Module> createEnergyModule(const std::string& name, int variant = 0) override;
    std::unique_ptr<Module> createResearchModule(const std::string& name, int variant = 0) override;
};

class AdvancedModuleFactory : public ModuleFactory
{
public:
    std::unique_ptr<Module> createResidentialModule(const std::string& name, int variant = 0) override;
    std::unique_ptr<Module> createEnergyModule(const std::string& name, int variant = 0) override;
    std::unique_ptr<Module> createResearchModule(const std::string& name, int variant = 0) override;
};