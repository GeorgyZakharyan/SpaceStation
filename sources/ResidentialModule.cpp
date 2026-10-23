#include "ResidentialModule.h"

ResidentialModule::ResidentialModule(const std::string& name, int durability, int energy, int capacity, int efficiency)
    : Module(name, durability, energy, capacity, efficiency) {}

std::string ResidentialModule::getCategory() const
{
    return "Жилой";
}

CrewCabin::CrewCabin(const std::string& name)
    : ResidentialModule(name, 100, 10, 4, 80) {}

std::string CrewCabin::getType() const
{
    return "CrewCabin";
}

std::unique_ptr<Module> CrewCabin::clone() const
{
    return std::unique_ptr<Module>(new CrewCabin(*this));
}

MedicalCabin::MedicalCabin(const std::string& name)
    : ResidentialModule(name, 100, 20, 2, 90) {}

std::string MedicalCabin::getType() const
{
    return "MedicalCabin";
}

std::unique_ptr<Module> MedicalCabin::clone() const
{
    return std::unique_ptr<Module>(new MedicalCabin(*this));
}

Coworking::Coworking(const std::string& name)
    : ResidentialModule(name, 100, 15, 6, 70) {}

std::string Coworking::getType() const
{
    return "Coworking";
}

std::unique_ptr<Module> Coworking::clone() const
{
    return std::unique_ptr<Module>(new Coworking(*this));
}

GalleyModule::GalleyModule(const std::string& name)
    : ResidentialModule(name, 100, 25, 10, 75) {}

std::string GalleyModule::getType() const
{
    return "GalleyModule";
}

std::unique_ptr<Module> GalleyModule::clone() const
{
    return std::unique_ptr<Module>(new GalleyModule(*this));
}