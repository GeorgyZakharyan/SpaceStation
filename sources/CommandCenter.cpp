#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "CommandCenter.h"
#include "ModuleFactory.h"
#include "ExternalObject.h"
#include "LoggingProxy.h"

CommandCenter::CommandCenter(const std::string& name, size_t maxModules, int energy, int oxygen, int safetyLevel)
    : name_(name), energy_(energy < 0 ? 0 : energy), oxygen_(oxygen < 0 ? 0 : oxygen), safetyLevel_(0),
      radiation_(0), maxModules_(maxModules < 1 ? 1 : maxModules), eventCount_(0)
{
    setSafetyLevel(safetyLevel);
}

void CommandCenter::cloneModulesFrom(const CommandCenter& other)
{
    modules_.clear();
    for (size_t i = 0; i < other.modules_.size(); i++)
    {
        modules_.push_back(other.modules_[i]->clone());
        modules_.back()->subscribe(this);
    }
}

CommandCenter::CommandCenter(const CommandCenter& other)
    : IObserver(other), name_(other.name_), energy_(other.energy_), oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_), radiation_(other.radiation_), maxModules_(other.maxModules_),
      destroyedIds_(other.destroyedIds_), eventCount_(other.eventCount_)
{
    cloneModulesFrom(other);
    if (other.currentEnvironment_ != nullptr)
    {
        currentEnvironment_ = std::make_shared<EnvironmentProxy>(other.currentEnvironment_->getRealEnvironment());
    }
}

CommandCenter& CommandCenter::operator=(const CommandCenter& other)
{
    if (this == &other)
    {
        return *this;
    }
    name_ = other.name_;
    energy_ = other.energy_;
    oxygen_ = other.oxygen_;
    safetyLevel_ = other.safetyLevel_;
    radiation_ = other.radiation_;
    maxModules_ = other.maxModules_;
    destroyedIds_ = other.destroyedIds_;
    eventCount_ = other.eventCount_;
    cloneModulesFrom(other);
    if (other.currentEnvironment_ != nullptr)
    {
        currentEnvironment_ = std::make_shared<EnvironmentProxy>(other.currentEnvironment_->getRealEnvironment());
    }
    else
    {
        currentEnvironment_ = nullptr;
    }
    return *this;
}

CommandCenter::CommandCenter(CommandCenter&& other)
    : IObserver(other), name_(std::move(other.name_)), energy_(other.energy_), oxygen_(other.oxygen_),
      safetyLevel_(other.safetyLevel_), radiation_(other.radiation_), maxModules_(other.maxModules_),
      modules_(std::move(other.modules_)), currentEnvironment_(std::move(other.currentEnvironment_)),
      destroyedIds_(std::move(other.destroyedIds_)), eventCount_(other.eventCount_)
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        modules_[i]->unsubscribe(&other);
        modules_[i]->subscribe(this);
    }
    other.modules_.clear();
    other.destroyedIds_.clear();
    other.energy_ = 0;
    other.oxygen_ = 0;
    other.maxModules_ = 0;
}

CommandCenter& CommandCenter::operator=(CommandCenter&& other)
{
    if (this == &other)
    {
        return *this;
    }
    name_ = std::move(other.name_);
    energy_ = other.energy_;
    oxygen_ = other.oxygen_;
    safetyLevel_ = other.safetyLevel_;
    radiation_ = other.radiation_;
    maxModules_ = other.maxModules_;
    eventCount_ = other.eventCount_;
    modules_ = std::move(other.modules_);
    currentEnvironment_ = std::move(other.currentEnvironment_);
    destroyedIds_ = std::move(other.destroyedIds_);

    for (size_t i = 0; i < modules_.size(); i++)
    {
        modules_[i]->unsubscribe(&other);
        modules_[i]->subscribe(this);
    }
    other.modules_.clear();
    other.destroyedIds_.clear();
    other.energy_ = 0;
    other.oxygen_ = 0;
    other.maxModules_ = 0;
    return *this;
}

void CommandCenter::updatePositions()
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        modules_[i]->moveToIndex((int)i);
    }
}

void CommandCenter::setSafetyLevel(int newLevel)
{
    if (newLevel < 0)
    {
        newLevel = 0;
    }
    if (newLevel > 100)
    {
        newLevel = 100;
    }
    safetyLevel_ = newLevel;
}

bool CommandCenter::addModule(std::unique_ptr<Module> module)
{
    if (module == nullptr)
    {
        return false;
    }
    if (modules_.size() >= maxModules_)
    {
        LoggingProxy::log(LogSource::CENTER, "Нельзя добавить " + module->getName()
                          + ": достигнут максимум модулей (" + std::to_string(maxModules_) + ")");
        return false;
    }

    module->setTypeInfo(ModuleTypeInfoFactory::getInfo(*module));
    module->subscribe(this);

    LoggingProxy::log(LogSource::CENTER, "Добавлен модуль " + module->getName() + " (" + module->getType() + ")");
    modules_.push_back(std::move(module));
    updatePositions();
    return true;
}

bool CommandCenter::createResidentialModule(ModuleFactory& factory, const std::string& name, int variant)
{
    return addModule(factory.createResidentialModule(name, variant));
}

bool CommandCenter::createEnergyModule(ModuleFactory& factory, const std::string& name, int variant)
{
    return addModule(factory.createEnergyModule(name, variant));
}

bool CommandCenter::createResearchModule(ModuleFactory& factory, const std::string& name, int variant)
{
    return addModule(factory.createResearchModule(name, variant));
}

bool CommandCenter::removeById(int id)
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getId() == id)
        {
            LoggingProxy::log(LogSource::CENTER, "Модуль " + modules_[i]->getName() + " удалён из списка станции");
            modules_.erase(modules_.begin() + i);
            updatePositions();
            return true;
        }
    }
    return false;
}

void CommandCenter::removeDestroyedModules()
{
    for (size_t i = 0; i < destroyedIds_.size(); i++)
    {
        removeById(destroyedIds_[i]);
    }
    destroyedIds_.clear();
}

Module* CommandCenter::findById(int id) const
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getId() == id)
        {
            return modules_[i].get();
        }
    }
    return nullptr;
}

Module* CommandCenter::getModule(int index) const
{
    if (index < 0 || index >= (int)modules_.size())
    {
        return nullptr;
    }
    return modules_[index].get();
}

int CommandCenter::getModuleCount() const
{
    return (int)modules_.size();
}

bool CommandCenter::teleportModule(int id)
{
    int from = -1;
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getId() == id)
        {
            from = (int)i;
        }
    }
    if (from == -1 || modules_.size() < 2)
    {
        return false;
    }

    int to = std::rand() % ((int)modules_.size() - 1);
    if (to >= from)
    {
        to++;
    }
    std::unique_ptr<Module> temp = std::move(modules_[from]);
    modules_.erase(modules_.begin() + from);
    modules_.insert(modules_.begin() + to, std::move(temp));
    updatePositions();

    LoggingProxy::log(LogSource::CENTER, "Модуль id=" + std::to_string(id) + " телепортирован: позиция "
                      + std::to_string(from) + " -> " + std::to_string(to));
    return true;
}

bool CommandCenter::transferEnergy(int fromId, int toId, int amount)
{
    Module* from = findById(fromId);
    Module* to = findById(toId);
    if (from == nullptr || to == nullptr)
    {
        return false;
    }
    return from->transferEnergyTo(*to, amount);
}

const std::string& CommandCenter::getName() const
{
    return name_;
}

int CommandCenter::getEnergy() const
{
    return energy_;
}

int CommandCenter::getOxygen() const
{
    return oxygen_;
}

int CommandCenter::getSafetyLevel() const
{
    return safetyLevel_;
}

int CommandCenter::getRadiation() const
{
    return radiation_;
}

size_t CommandCenter::getMaxModules() const
{
    return maxModules_;
}

int CommandCenter::getEventCount() const
{
    return eventCount_;
}

void CommandCenter::addResources(const ResourcePackage& package)
{
    energy_ += package.getEnergy();
    oxygen_ += package.getOxygen();
    setSafetyLevel(safetyLevel_ + package.getSafetyLevel());
    LoggingProxy::log(LogSource::CENTER, "Ресурсы получены: энергия " + std::to_string(energy_)
                      + ", кислород " + std::to_string(oxygen_) + ", безопасность " + std::to_string(safetyLevel_));
}

void CommandCenter::addRadiation(int amount)
{
    if (amount > 0)
    {
        radiation_ += amount;
    }
}

void CommandCenter::setEnvironment(const std::shared_ptr<Environment>& environment)
{
    if (environment == nullptr)
    {
        return;
    }

    for (size_t i = 0; i < modules_.size(); i++)
    {
        modules_[i]->setBlocked(false);
    }

    currentEnvironment_ = std::make_shared<EnvironmentProxy>(environment);
    LoggingProxy::log(LogSource::CENTER, "Установлено окружение: " + environment->getName());
    applyEnvironment();
}

std::string CommandCenter::getEnvironmentName() const
{
    if (currentEnvironment_ == nullptr)
    {
        return "не задано";
    }
    return currentEnvironment_->getName();
}

double CommandCenter::getEnergyModifier() const
{
    if (currentEnvironment_ == nullptr)
    {
        return 1.0;
    }
    return currentEnvironment_->getEnergyModifier();
}

double CommandCenter::getDamageModifier() const
{
    if (currentEnvironment_ == nullptr)
    {
        return 1.0;
    }
    return currentEnvironment_->getDamageModifier();
}

int CommandCenter::calculateDamage(int baseAmount) const
{
    double result = baseAmount * getDamageModifier();
    result = result * (100 - safetyLevel_ / 2.0) / 100.0;

    int amount = (int)result;
    if (baseAmount > 0 && amount < 1)
    {
        amount = 1;
    }
    return amount;
}

void CommandCenter::applyEnvironment()
{
    if (currentEnvironment_ == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < modules_.size(); i++)
    {
        currentEnvironment_->applyEffect(*modules_[i]);
    }
    removeDestroyedModules();
}

std::shared_ptr<EnvironmentProxy> CommandCenter::getEnvironmentProxy() const
{
    return currentEnvironment_;
}

void CommandCenter::tick()
{
    LoggingProxy::log(LogSource::CENTER, "--- Новый ход ---");

    double energyModifier = getEnergyModifier();
    int oxygenUsed = 0;

    for (size_t i = 0; i < modules_.size(); i++)
    {
        Module& module = *modules_[i];
        if (!module.isActive())
        {
            continue;
        }
        int amount = (int)(module.getEnergy().getValue() * energyModifier);
        module.consumeEnergy(amount);

        if (module.needsOxygen())
        {
            oxygenUsed += module.getCapacity().getValue();
        }
    }

    oxygen_ -= oxygenUsed;
    if (oxygen_ < 0)
    {
        oxygen_ = 0;
    }
    if (oxygenUsed > 0 && oxygen_ == 0)
    {
        LoggingProxy::log(LogSource::CENTER, "Кислород закончился!");
        setSafetyLevel(safetyLevel_ - 2);
    }

    if (currentEnvironment_ != nullptr)
    {
        currentEnvironment_->clearCache();
    }
    applyEnvironment();

    if (radiation_ > 0)
    {
        radiation_--;
    }

    if (energy_ == 0)
    {
        LoggingProxy::log(LogSource::CENTER, "Энергия закончилась, все модули отключены");
        for (size_t i = 0; i < modules_.size(); i++)
        {
            modules_[i]->deactivate();
        }
    }
}

bool CommandCenter::receiveObject(ExternalObject& object, int moduleId)
{
    Module* module = findById(moduleId);
    if (module == nullptr)
    {
        LoggingProxy::log(LogSource::CENTER, "Модуль id=" + std::to_string(moduleId) + " не найден");
        return false;
    }

    LoggingProxy::log(LogSource::CENTER, "Обнаружен объект: " + object.getType());
    object.interact(*this, *module);
    removeDestroyedModules();
    return true;
}

void CommandCenter::onModuleEvent(const ModuleEvent& event)
{
    eventCount_++;
    std::string id = std::to_string(event.getModuleId());

    switch (event.getType())
    {
        case EVENT_ACTIVATED:
            LoggingProxy::log(LogSource::CENTER, "Уведомление #" + std::to_string(event.getTimestamp())
                              + ": модуль id=" + id + " активирован");
            break;

        case EVENT_DAMAGED:
            setSafetyLevel(safetyLevel_ - 1);
            LoggingProxy::log(LogSource::CENTER, "Уведомление #" + std::to_string(event.getTimestamp())
                              + ": модуль id=" + id + " получил урон " + std::to_string(event.getDamage())
                              + ", безопасность " + std::to_string(safetyLevel_));
            break;

        case EVENT_DESTROYED:
            setSafetyLevel(safetyLevel_ - 5);
            destroyedIds_.push_back(event.getModuleId());
            LoggingProxy::log(LogSource::CENTER, "Уведомление #" + std::to_string(event.getTimestamp())
                              + ": модуль id=" + id + " РАЗРУШЕН, безопасность " + std::to_string(safetyLevel_));
            break;

        case EVENT_ENERGY_CONSUMED:
            energy_ -= event.getDamage();       
            if (energy_ < 0)
            {
                energy_ = 0;
            }
            LoggingProxy::log(LogSource::CENTER, "Уведомление #" + std::to_string(event.getTimestamp())
                              + ": модуль id=" + id + " потратил энергию " + std::to_string(event.getDamage())
                              + ", запас станции " + std::to_string(energy_));
            break;
    }
}

void CommandCenter::showStatistics() const
{
    int totalCost = 0;
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getTypeInfo() != nullptr)
        {
            totalCost += modules_[i]->getTypeInfo()->getCost();
        }
    }

    std::cout << "Командный центр \"" << name_ << "\"" << std::endl;
    std::cout << "  Модулей: " << modules_.size() << "/" << maxModules_
              << ", окружение: " << getEnvironmentName() << std::endl;
    std::cout << "  Энергия: " << energy_ << ", кислород: " << oxygen_
              << ", безопасность: " << safetyLevel_ << ", радиация: " << radiation_ << std::endl;
    std::cout << "  Получено уведомлений: " << eventCount_
              << ", стоимость модулей: " << totalCost
              << ", общих описаний типов (легковес): " << ModuleTypeInfoFactory::getCount() << std::endl;
    for (size_t i = 0; i < modules_.size(); i++)
    {
        std::cout << "    ";
        modules_[i]->showInfo();
    }
}
