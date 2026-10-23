#pragma once
#include "Module.h"

class ResidentialModule : public Module
{
public:
    ResidentialModule(const std::string& name, int durability, int energy, int capacity, int efficiency);
    std::string getCategory() const override;
};

class CrewCabin : public ResidentialModule
{
public:
    CrewCabin(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class MedicalCabin : public ResidentialModule
{
public:
    MedicalCabin(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class Coworking : public ResidentialModule
{
public:
    Coworking(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};

class GalleyModule : public ResidentialModule
{
public:
    GalleyModule(const std::string& name);
    std::string getType() const override;
    std::unique_ptr<Module> clone() const override;
};