#pragma once
#include "Module.h"

class ResearchModule : public Module
{
public:
    ResearchModule(const std::string& name, int durability, int energy, int capacity, int efficiency);
    std::string getCategory() const override;
};

class BiologyLab : public ResearchModule
{
public:
    BiologyLab(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class PhysicsLab : public ResearchModule
{
public:
    PhysicsLab(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class AstronomyLab : public ResearchModule
{
public:
    AstronomyLab(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class Workshop : public ResearchModule
{
public:
    Workshop(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};