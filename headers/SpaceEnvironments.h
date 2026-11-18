#pragma once
#include "Environment.h"

class EarthOrbit : public Environment
{
public:
    void applyEffect(Module& module) override;
    std::string getName() const override;
    double getEnergyModifier() const override;
    double getDamageModifier() const override;
};

class DeepSpace : public Environment
{
public:
    void applyEffect(Module& module) override;
    std::string getName() const override;
    double getEnergyModifier() const override;
    double getDamageModifier() const override;
};

class AsteroidField : public Environment
{
public:
    void applyEffect(Module& module) override;
    std::string getName() const override;
    double getEnergyModifier() const override;
    double getDamageModifier() const override;
};
