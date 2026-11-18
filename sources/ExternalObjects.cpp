#include <cstdlib>
#include <memory>
#include "ExternalObjects.h"
#include "CommandCenter.h"
#include "Module.h"
#include "Strategies.h"
#include "Damage.h"
#include "ResourcePackage.h"
#include "Operators.h"
#include "LoggingProxy.h"

SolarStorm::SolarStorm(int strength) : ExternalObject("Солнечная буря", strength) {}

void SolarStorm::interact(CommandCenter& center, Module& module)
{
    int power = center.calculateDamage(strength_);
    center.addRadiation(strength_ / 10);

    std::unique_ptr<IInteractionStrategy> strategy = chooseStrategy(module, power);
    LoggingProxy::log(LogSource::OBJECT, type_ + " бьёт по модулю " + module.getName()
                      + " (сила " + std::to_string(power) + ", " + strategy->getName() + ")");
    strategy->apply(module);
}

MeteorShower::MeteorShower(int strength) : ExternalObject("Метеоритный поток", strength) {}

void MeteorShower::interact(CommandCenter& center, Module& module)
{
    int hitDamage = center.calculateDamage(strength_ / 3);
    LoggingProxy::log(LogSource::OBJECT, type_ + ": три удара по модулю " + module.getName()
                      + ", по " + std::to_string(hitDamage) + " урона");

    for (int i = 0; i < 3; i++)
    {
        module - Damage(hitDamage, "физический удар");
    }
}

CargoContainer::CargoContainer(int strength) : ExternalObject("Грузовой контейнер", strength) {}

void CargoContainer::interact(CommandCenter& center, Module&)
{
    ResourcePackage package(strength_ * 2, strength_, 2);
    LoggingProxy::log(LogSource::OBJECT, type_ + " доставлен: энергия +" + std::to_string(package.getEnergy())
                      + ", кислород +" + std::to_string(package.getOxygen())
                      + ", безопасность +" + std::to_string(package.getSafetyLevel()));
    center + package;
}

SpaceAnomaly::SpaceAnomaly(int strength) : ExternalObject("Космическая аномалия", strength) {}

void SpaceAnomaly::interact(CommandCenter& center, Module& module)
{
    int effect = std::rand() % 3;
    interactWithEffect(center, module, effect);
}

void SpaceAnomaly::interactWithEffect(CommandCenter& center, Module& module, int effect)
{
    if (effect == 0)
    {
        LoggingProxy::log(LogSource::OBJECT, type_ + ": телепортация модуля " + module.getName()
                          + " (после прыжка калибровка сбита)");
        center.teleportModule(module.getId());
        module * 0.9;       
    }
    else if (effect == 1)
    {
        LoggingProxy::log(LogSource::OBJECT, type_ + ": энергия станции удваивается, модуль " + module.getName() + " усилен");
        center + ResourcePackage(center.getEnergy(), 0, 0);     
        module * 1.5;
    }
    else
    {
        LoggingProxy::log(LogSource::OBJECT, type_ + ": модуль " + module.getName() + " отключён");
        module * 0.0;
        module.deactivate();
    }
}
