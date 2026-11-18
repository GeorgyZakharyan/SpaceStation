#pragma once
#include <string>

class Module;

class IInteractionStrategy
{
public:
    virtual ~IInteractionStrategy() {}

    virtual void apply(Module& module) = 0;
    virtual std::string getName() const = 0;
};
