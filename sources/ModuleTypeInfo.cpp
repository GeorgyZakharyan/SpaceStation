#include "ModuleTypeInfo.h"
#include "Module.h"

ModuleTypeInfo::ModuleTypeInfo(int maxHealth, int baseEnergyConsumption, int baseEfficiency, int cost)
    : maxHealth_(maxHealth), baseEnergyConsumption_(baseEnergyConsumption), baseEfficiency_(baseEfficiency), cost_(cost) {}

int ModuleTypeInfo::getMaxHealth() const
{
    return maxHealth_;
}

int ModuleTypeInfo::getBaseEnergyConsumption() const
{
    return baseEnergyConsumption_;
}

int ModuleTypeInfo::getBaseEfficiency() const
{
    return baseEfficiency_;
}

int ModuleTypeInfo::getCost() const
{
    return cost_;
}

std::map<std::string, std::shared_ptr<const ModuleTypeInfo> > ModuleTypeInfoFactory::infos_;

std::shared_ptr<const ModuleTypeInfo> ModuleTypeInfoFactory::getInfo(const Module& module)
{
    std::string type = module.getType();

    std::map<std::string, std::shared_ptr<const ModuleTypeInfo> >::iterator it = infos_.find(type);
    if (it != infos_.end())
    {
        return it->second;      
    }

    int maxHealth = module.getDurability().getMax();
    int energy = module.getEnergy().getValue();
    int efficiency = module.getEfficiency();
    int cost = energy * 10 + maxHealth;     

    std::shared_ptr<const ModuleTypeInfo> info(new ModuleTypeInfo(maxHealth, energy, efficiency, cost));
    infos_[type] = info;
    return info;
}

int ModuleTypeInfoFactory::getCount()
{
    return (int)infos_.size();
}
