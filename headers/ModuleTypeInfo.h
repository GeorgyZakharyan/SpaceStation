#pragma once
#include <map>
#include <memory>
#include <string>

class Module;

class ModuleTypeInfo
{
private:
    int maxHealth_;
    int baseEnergyConsumption_;
    int baseEfficiency_;
    int cost_;

public:
    ModuleTypeInfo(int maxHealth, int baseEnergyConsumption, int baseEfficiency, int cost);

    int getMaxHealth() const;
    int getBaseEnergyConsumption() const;
    int getBaseEfficiency() const;
    int getCost() const;
};

class ModuleTypeInfoFactory
{
private:
    static std::map<std::string, std::shared_ptr<const ModuleTypeInfo> > infos_;

public:
    static std::shared_ptr<const ModuleTypeInfo> getInfo(const Module& module);
    static int getCount();
};
