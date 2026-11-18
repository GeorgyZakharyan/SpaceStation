#include <memory>
#include "SpaceEnvironments.h"
#include "Module.h"
#include "Strategies.h"
#include "LoggingProxy.h"

void EarthOrbit::applyEffect(Module& module)
{
    LoggingProxy::log(LogSource::ENV, "Орбита Земли: на модуль " + module.getName() + " почти ничего не влияет");
}

std::string EarthOrbit::getName() const
{
    return "Орбита Земли";
}

double EarthOrbit::getEnergyModifier() const
{
    return 1.0;
}

double EarthOrbit::getDamageModifier() const
{
    return 1.0;
}

void DeepSpace::applyEffect(Module& module)
{
    int power = (int)(4 * getDamageModifier());
    std::unique_ptr<IInteractionStrategy> strategy = chooseStrategy(module, power);

    LoggingProxy::log(LogSource::ENV, "Глубокий космос: радиация, модуль " + module.getName()
                      + ", стратегия " + strategy->getName());
    strategy->apply(module);
}

std::string DeepSpace::getName() const
{
    return "Глубокий космос";
}

double DeepSpace::getEnergyModifier() const
{
    return 1.5;
}

double DeepSpace::getDamageModifier() const
{
    return 1.5;
}

void AsteroidField::applyEffect(Module& module)
{
    int power = (int)(5 * getDamageModifier());
    std::unique_ptr<IInteractionStrategy> strategy = chooseStrategy(module, power);

    LoggingProxy::log(LogSource::ENV, "Астероидное поле: удары, модуль " + module.getName()
                      + ", стратегия " + strategy->getName());
    strategy->apply(module);

    if (module.isResearch() && !module.isDestroyed())
    {
        module.setBlocked(true);
        LoggingProxy::log(LogSource::ENV, "Астероидное поле: исследовательский модуль " + module.getName() + " заблокирован");
    }
}

std::string AsteroidField::getName() const
{
    return "Астероидное поле";
}

double AsteroidField::getEnergyModifier() const
{
    return 1.0;
}

double AsteroidField::getDamageModifier() const
{
    return 2.0;
}
