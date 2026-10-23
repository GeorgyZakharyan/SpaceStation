#pragma once
#include "Module.h"

class EnergyModule : public Module
{
public:
    EnergyModule(const std::string& name, int durability, int energy, int capacity, int efficiency);
    std::string getCategory() const override;
};

class SolarModule : public EnergyModule
{
public:
    SolarModule(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class ReactorModule : public EnergyModule
{
public:
    ReactorModule(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class BackupGeneratorModule : public EnergyModule
{
public:
    BackupGeneratorModule(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};