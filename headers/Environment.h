#pragma once
#include <string>

class Module;

class Environment
{
public:
    virtual ~Environment();

    virtual void applyEffect(Module& module) = 0;       
    virtual std::string getName() const = 0;            
    virtual double getEnergyModifier() const = 0;       
    virtual double getDamageModifier() const = 0;       
};
