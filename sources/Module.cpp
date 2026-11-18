#include <iostream>
#include "Module.h"
#include "LoggingProxy.h"

int Module::nextId_ = 1;

Module::Module(const std::string& name, int durability, int energy, int capacity, int efficiency)
    : id_(nextId_++), name_(name), durability_(durability), energyConsumption_(energy), capacity_(capacity),
      efficiency_(0), position_(-1), active_(false), state_(ON), blocked_(false)
{
    setEfficiency(efficiency);
}

Module::~Module() {}

bool Module::needsOxygen() const
{
    return false;
}

int Module::getId() const
{
    return id_;
}

const std::string& Module::getName() const
{
    return name_;
}

int Module::getPosition() const
{
    return position_;
}

int Module::getEfficiency() const
{
    return efficiency_;
}

bool Module::isActive() const
{
    return active_;
}

ModuleState Module::getState() const
{
    return state_;
}

std::string Module::getStateName() const
{
    switch (state_)
    {
        case ON:
            return "включён";
        case REPAIR:
            return "ремонт";
        case EMERGENCY:
            return "авария";
    }
    return "неизвестно";
}

Durability& Module::getDurability()
{
    return durability_;
}

EnergyConsumption& Module::getEnergy()
{
    return energyConsumption_;
}

Capacity& Module::getCapacity()
{
    return capacity_;
}

const Durability& Module::getDurability() const
{
    return durability_;
}

const EnergyConsumption& Module::getEnergy() const
{
    return energyConsumption_;
}

const Capacity& Module::getCapacity() const
{
    return capacity_;
}

bool Module::isResearch() const
{
    return getCategory() == "Исследовательский";
}

bool Module::isDestroyed() const
{
    return durability_.getValue() == 0;
}

bool Module::isBlocked() const
{
    return blocked_;
}

void Module::setBlocked(bool blocked)
{
    if (blocked_ == blocked)
    {
        return;     
    }
    blocked_ = blocked;
    if (blocked_)
    {
        active_ = false;
        LoggingProxy::log(LogSource::MODULE, name_ + ": заблокирован");
    }
    else
    {
        LoggingProxy::log(LogSource::MODULE, name_ + ": блокировка снята");
    }
}

const ModuleTypeInfo* Module::getTypeInfo() const
{
    return typeInfo_.get();
}

void Module::setTypeInfo(const std::shared_ptr<const ModuleTypeInfo>& info)
{
    typeInfo_ = info;
}

void Module::setEfficiency(int newEfficiency)
{
    if (newEfficiency < 0)
    {
        newEfficiency = 0;
    }
    if (newEfficiency > 100)
    {
        newEfficiency = 100;
    }
    efficiency_ = newEfficiency;
}

void Module::moveToIndex(int newPosition)
{
    position_ = newPosition;
}

void Module::activate()
{
    if (blocked_)
    {
        LoggingProxy::log(LogSource::MODULE, name_ + ": нельзя активировать, модуль заблокирован окружением");
        return;
    }
    if (state_ != ON)
    {
        LoggingProxy::log(LogSource::MODULE, name_ + ": нельзя активировать, состояние: " + getStateName());
        return;
    }
    if (active_)
    {
        return;     
    }
    active_ = true;
    LoggingProxy::log(LogSource::MODULE, name_ + ": активирован");
    notify(ModuleEvent(EVENT_ACTIVATED, id_));
}

void Module::deactivate()
{
    if (!active_)
    {
        return;
    }
    active_ = false;
    LoggingProxy::log(LogSource::MODULE, name_ + ": отключён");
}

void Module::setState(ModuleState newState)
{
    if (state_ == newState)
    {
        return;
    }
    state_ = newState;
    LoggingProxy::log(LogSource::MODULE, name_ + ": состояние изменилось, теперь: " + getStateName());
    if (state_ != ON)
    {
        active_ = false;
    }
}

void Module::takeDamage(int amount)
{
    if (amount <= 0 || isDestroyed())
    {
        return;
    }

    int before = durability_.getValue();
    durability_.decrease(amount);
    int realDamage = before - durability_.getValue();   
    LoggingProxy::log(LogSource::MODULE, name_ + ": урон " + std::to_string(realDamage) + ", прочность "
                      + std::to_string(before) + " -> " + std::to_string(durability_.getValue()));
    notify(ModuleEvent(EVENT_DAMAGED, id_, realDamage));

    if (isDestroyed())
    {
        setState(EMERGENCY);
        LoggingProxy::log(LogSource::MODULE, name_ + ": модуль разрушен!");
        notify(ModuleEvent(EVENT_DESTROYED, id_, realDamage));
    }
}

void Module::consumeEnergy(int amount)
{
    if (amount <= 0)
    {
        return;
    }
    notify(ModuleEvent(EVENT_ENERGY_CONSUMED, id_, amount));
}

bool Module::transferEnergyTo(Module& other, int amount)
{
    if (this == &other || amount <= 0 || !active_ || isDestroyed() || other.isDestroyed())
    {
        return false;
    }

    int part = amount;
    if (part > other.energyConsumption_.getValue())
    {
        part = other.energyConsumption_.getValue();     
    }
    if (part == 0)
    {
        return false;
    }

    other.energyConsumption_.decrease(part);
    energyConsumption_.increase(part);
    LoggingProxy::log(LogSource::MODULE, name_ + " берёт на себя " + std::to_string(part) + " кВт нагрузки модуля " + other.name_);
    return true;
}

void Module::showInfo() const
{
    std::cout << "[" << position_ << "] id=" << id_
              << " " << name_ << " (" << getType() << ", " << getCategory() << ")"
              << ", прочность=" << durability_.getValue() << "/" << durability_.getMax()
              << ", энергия=" << energyConsumption_.getValue() << " кВт"
              << ", вместимость=" << capacity_.getValue()
              << ", эффективность=" << efficiency_ << "%"
              << ", состояние=" << getStateName()
              << ", " << (active_ ? "активен" : "не активен");
    if (blocked_)
    {
        std::cout << ", заблокирован";
    }
    std::cout << std::endl;
}
