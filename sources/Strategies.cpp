#include "Strategies.h"
#include "Module.h"
#include "Damage.h"
#include "Operators.h"

EnergyModuleStrategy::EnergyModuleStrategy(int power) : power_(power) {}

void EnergyModuleStrategy::apply(Module& module)
{
    module - Damage(power_ / 2, "радиация");    
    module * 0.7;                               
}

std::string EnergyModuleStrategy::getName() const
{
    return "EnergyModuleStrategy";
}

CrewModuleStrategy::CrewModuleStrategy(int power) : power_(power) {}

void CrewModuleStrategy::apply(Module& module)
{
    module - Damage(power_, "радиация");
    module * 0.9;
}

std::string CrewModuleStrategy::getName() const
{
    return "CrewModuleStrategy";
}

ResearchModuleStrategy::ResearchModuleStrategy(int power) : power_(power) {}

void ResearchModuleStrategy::apply(Module& module)
{
    module - Damage(power_ / 2, "радиация");
    module * 0.8;
}

std::string ResearchModuleStrategy::getName() const
{
    return "ResearchModuleStrategy";
}

ReactorModuleStrategy::ReactorModuleStrategy(int power) : power_(power) {}

void ReactorModuleStrategy::apply(Module& module)
{
    int amount = power_ / 4;
    if (amount < 1)
    {
        amount = 1;     
    }
    module - Damage(amount, "радиация");
}

std::string ReactorModuleStrategy::getName() const
{
    return "ReactorModuleStrategy";
}

DefaultModuleStrategy::DefaultModuleStrategy(int power) : power_(power) {}

void DefaultModuleStrategy::apply(Module& module)
{
    module - Damage(power_, "общее");
}

std::string DefaultModuleStrategy::getName() const
{
    return "DefaultModuleStrategy";
}

std::unique_ptr<IInteractionStrategy> chooseStrategy(const Module& module, int power)
{
    if (module.getType() == "ReactorModule")
    {
        return std::unique_ptr<IInteractionStrategy>(new ReactorModuleStrategy(power));
    }
    if (module.getCategory() == "Энергетический")
    {
        return std::unique_ptr<IInteractionStrategy>(new EnergyModuleStrategy(power));
    }
    if (module.getCategory() == "Жилой")
    {
        return std::unique_ptr<IInteractionStrategy>(new CrewModuleStrategy(power));
    }
    if (module.isResearch())
    {
        return std::unique_ptr<IInteractionStrategy>(new ResearchModuleStrategy(power));
    }
    return std::unique_ptr<IInteractionStrategy>(new DefaultModuleStrategy(power));
}
