#include "Research.h"

ResearchModule::ResearchModule(const std::string& name, int durability, int energy, int capacity, int efficiency)
    : Module(name, durability, energy, capacity, efficiency) {}

std::string ResearchModule::getCategory() const
{
    return "Исследовательский";
}

BiologyLab::BiologyLab(const std::string& name): ResearchModule(name, 100, 30, 20, 75) {}

std::string BiologyLab::getType() const
{
    return "BiologyLab";
}

std::unique_ptr<Module> BiologyLab::clone() const
{
    return std::unique_ptr<Module>(new BiologyLab(*this));
}

PhysicsLab::PhysicsLab(const std::string& name): ResearchModule(name, 100, 50, 10, 85) {}

std::string PhysicsLab::getType() const
{
    return "PhysicsLab";
}

std::unique_ptr<Module> PhysicsLab::clone() const
{
    return std::unique_ptr<Module>(new PhysicsLab(*this));
}

AstronomyLab::AstronomyLab(const std::string& name): ResearchModule(name, 100, 35, 15, 80) {}

std::string AstronomyLab::getType() const
{
    return "AstronomyLab";
}

std::unique_ptr<Module> AstronomyLab::clone() const
{
    return std::unique_ptr<Module>(new AstronomyLab(*this));
}

Workshop::Workshop(const std::string& name): ResearchModule(name, 100, 40, 5, 70) {}

std::string Workshop::getType() const
{
    return "Workshop";
}

std::unique_ptr<Module> Workshop::clone() const
{
    return std::unique_ptr<Module>(new Workshop(*this));
}