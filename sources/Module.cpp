#include <iostream>
#include "Module.h"

int Module::nextId_ = 1;   

Module::Module(const std::string& name, int durability, int energy, int capacity, int efficiency)
    : id_(nextId_++), name_(name), durability_(durability), energyConsumption_(energy), capacity_(capacity),
      efficiency_(0), position_(-1), active_(false), state_(ON)
{
    setEfficiency(efficiency);
}

Module::~Module() {}

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
    if (state_ != ON)
    {
        std::cout << "Модуль " << name_ << " нельзя активировать, состояние: " << getStateName() << std::endl;
        return;
    }
    active_ = true;
}

void Module::deactivate()
{
    active_ = false;
}

void Module::setState(ModuleState newState)
{
    state_ = newState;
    if (state_ != ON)
    {
        active_ = false;
    }
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
              << ", " << (active_ ? "активен" : "не активен")
              << std::endl;
}