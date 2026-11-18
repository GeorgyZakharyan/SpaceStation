#include "EnvironmentProxy.h"
#include "Module.h"
#include "LoggingProxy.h"

EnvironmentProxy::EnvironmentProxy(const std::shared_ptr<Environment>& realEnvironment)
    : realEnvironment_(realEnvironment) {}

void EnvironmentProxy::applyEffect(Module& module)
{
    if (realEnvironment_ == nullptr)
    {
        return;
    }

    if (module.isDestroyed())
    {
        LoggingProxy::log(LogSource::ENV, "Прокси: модуль " + module.getName() + " разрушен, эффект пропущен");
        return;
    }

    if (module.getState() == REPAIR)
    {
        LoggingProxy::log(LogSource::ENV, "Прокси: модуль " + module.getName() + " в ремонте, эффект пропущен");
        return;
    }

    if (module.isResearch() && module.isBlocked())
    {
        LoggingProxy::log(LogSource::ENV, "Прокси: исследовательский модуль " + module.getName()
                          + " заблокирован, доступ закрыт");
        return;
    }

    std::map<int, std::string>::iterator it = cache_.find(module.getId());
    if (it != cache_.end())
    {
        LoggingProxy::log(LogSource::ENV, "Прокси: из кеша для " + module.getName() + " (" + it->second + ")");
        return;
    }

    LoggingProxy::log(LogSource::ENV, "Прокси: проверки пройдены, передаю " + module.getName()
                      + " окружению \"" + realEnvironment_->getName() + "\"");
    int before = module.getDurability().getValue();
    realEnvironment_->applyEffect(module);
    int after = module.getDurability().getValue();

    cache_[module.getId()] = "прочность " + std::to_string(before) + " -> " + std::to_string(after);
}

std::string EnvironmentProxy::getName() const
{
    if (realEnvironment_ == nullptr)
    {
        return "нет окружения";
    }
    return realEnvironment_->getName();
}

double EnvironmentProxy::getEnergyModifier() const
{
    if (realEnvironment_ == nullptr)
    {
        return 1.0;
    }
    return realEnvironment_->getEnergyModifier();
}

double EnvironmentProxy::getDamageModifier() const
{
    if (realEnvironment_ == nullptr)
    {
        return 1.0;
    }
    return realEnvironment_->getDamageModifier();
}

std::shared_ptr<Environment> EnvironmentProxy::getRealEnvironment() const
{
    return realEnvironment_;
}

void EnvironmentProxy::clearCache()
{
    cache_.clear();
}

int EnvironmentProxy::getCacheSize() const
{
    return (int)cache_.size();
}
