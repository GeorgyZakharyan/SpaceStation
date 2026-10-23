#include "Energy.h"

EnergyModule::EnergyModule(const std::string& name, int durability, int energy, int capacity, int efficiency)
    : Module(name, durability, energy, capacity, efficiency) {}

std::string EnergyModule::getCategory() const
{
    return "Энергетический";
}

SolarModule::SolarModule(const std::string& name): EnergyModule(name, 100, 5, 50, 70) {}

std::string SolarModule::getType() const
{
    return "SolarModule";
}

std::unique_ptr<Module> SolarModule::clone() const
{
    return std::unique_ptr<Module>(new SolarModule(*this));
}

ReactorModule::ReactorModule(const std::string& name): EnergyModule(name, 100, 15, 200, 95) {}

std::string ReactorModule::getType() const
{
    return "ReactorModule";
}

std::unique_ptr<Module> ReactorModule::clone() const
{
    return std::unique_ptr<Module>(new ReactorModule(*this));
}

BackupGeneratorModule::BackupGeneratorModule(const std::string& name): EnergyModule(name, 100, 10, 30, 60) {}

std::string BackupGeneratorModule::getType() const
{
    return "BackupGeneratorModule";
}

std::unique_ptr<Module> BackupGeneratorModule::clone() const
{
    return std::unique_ptr<Module>(new BackupGeneratorModule(*this));
}