#include "ModuleFactory.h"
#include "ResidentialModule.h"   // ИЗМЕНЕНО: было HabitationModule.h
#include "Energy.h"
#include "Research.h"

ModuleFactory::~ModuleFactory() {}

std::unique_ptr<Module> StandardModuleFactory::createResidentialModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new GalleyModule(name));
    }
    return std::unique_ptr<Module>(new CrewCabin(name));
}

std::unique_ptr<Module> StandardModuleFactory::createEnergyModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new BackupGeneratorModule(name));
    }
    return std::unique_ptr<Module>(new SolarModule(name));
}

std::unique_ptr<Module> StandardModuleFactory::createResearchModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new Workshop(name));
    }
    return std::unique_ptr<Module>(new BiologyLab(name));
}

std::unique_ptr<Module> AdvancedModuleFactory::createResidentialModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new Coworking(name));
    }
    return std::unique_ptr<Module>(new MedicalCabin(name));
}

std::unique_ptr<Module> AdvancedModuleFactory::createEnergyModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new BackupGeneratorModule(name));
    }
    return std::unique_ptr<Module>(new ReactorModule(name));
}

std::unique_ptr<Module> AdvancedModuleFactory::createResearchModule(const std::string& name, int variant)
{
    if (variant == 1)
    {
        return std::unique_ptr<Module>(new AstronomyLab(name));
    }
    return std::unique_ptr<Module>(new PhysicsLab(name));
}