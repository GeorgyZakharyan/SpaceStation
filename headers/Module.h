#pragma once
#include <string>
#include <memory>
#include "Durability.h"
#include "EnergyConsumption.h"
#include "Capacity.h"
#include "IObservable.h"
#include "ModuleTypeInfo.h"

enum ModuleState
{
    ON, REPAIR, EMERGENCY
};

class Module : public IObservable
{
protected:
    int id_;
    std::string name_;
    Durability durability_;
    EnergyConsumption energyConsumption_;
    Capacity capacity_;
    int efficiency_;
    int position_;
    bool active_;
    ModuleState state_;
    bool blocked_;                                     
    std::shared_ptr<const ModuleTypeInfo> typeInfo_;    

    static int nextId_;

public:
    Module(const std::string& name, int durability, int energy, int capacity, int efficiency);
    virtual ~Module();

    virtual std::string getType() const = 0;
    virtual std::string getCategory() const = 0;
    virtual std::unique_ptr<Module> clone() const = 0;

    virtual void showInfo() const;

    virtual bool needsOxygen() const;

    int getId() const;
    const std::string& getName() const;
    int getPosition() const;
    int getEfficiency() const;
    bool isActive() const;
    ModuleState getState() const;
    std::string getStateName() const;
    Durability& getDurability();
    EnergyConsumption& getEnergy();
    Capacity& getCapacity();
    const Durability& getDurability() const;
    const EnergyConsumption& getEnergy() const;
    const Capacity& getCapacity() const;

    bool isResearch() const;
    bool isDestroyed() const;
    bool isBlocked() const;
    void setBlocked(bool blocked);

    const ModuleTypeInfo* getTypeInfo() const;
    void setTypeInfo(const std::shared_ptr<const ModuleTypeInfo>& info);

    void setEfficiency(int newEfficiency);
    void moveToIndex(int newPosition);

    void activate();
    void deactivate();
    void setState(ModuleState newState);

    void takeDamage(int amount);
    void consumeEnergy(int amount);

    bool transferEnergyTo(Module& other, int amount);
};
