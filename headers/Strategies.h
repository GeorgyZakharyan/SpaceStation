#pragma once
#include <memory>
#include "IInteractionStrategy.h"

class EnergyModuleStrategy : public IInteractionStrategy
{
private:
    int power_;

public:
    explicit EnergyModuleStrategy(int power);
    void apply(Module& module) override;
    std::string getName() const override;
};

class CrewModuleStrategy : public IInteractionStrategy
{
private:
    int power_;

public:
    explicit CrewModuleStrategy(int power);
    void apply(Module& module) override;
    std::string getName() const override;
};

class ResearchModuleStrategy : public IInteractionStrategy
{
private:
    int power_;

public:
    explicit ResearchModuleStrategy(int power);
    void apply(Module& module) override;
    std::string getName() const override;
};

class ReactorModuleStrategy : public IInteractionStrategy
{
private:
    int power_;

public:
    explicit ReactorModuleStrategy(int power);
    void apply(Module& module) override;
    std::string getName() const override;
};

class DefaultModuleStrategy : public IInteractionStrategy
{
private:
    int power_;

public:
    explicit DefaultModuleStrategy(int power);
    void apply(Module& module) override;
    std::string getName() const override;
};

std::unique_ptr<IInteractionStrategy> chooseStrategy(const Module& module, int power);
