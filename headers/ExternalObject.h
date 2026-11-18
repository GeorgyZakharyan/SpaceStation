#pragma once
#include <string>

class CommandCenter;
class Module;

class ExternalObject
{
protected:
    std::string type_;      
    int strength_;          

public:
    ExternalObject(const std::string& type, int strength);
    virtual ~ExternalObject();

    virtual void interact(CommandCenter& center, Module& module) = 0;

    const std::string& getType() const;
    int getStrength() const;
};
