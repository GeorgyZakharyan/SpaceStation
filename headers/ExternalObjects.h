#pragma once
#include "ExternalObject.h"

class SolarStorm : public ExternalObject
{
public:
    explicit SolarStorm(int strength = 30);
    void interact(CommandCenter& center, Module& module) override;
};

class MeteorShower : public ExternalObject
{
public:
    explicit MeteorShower(int strength = 25);
    void interact(CommandCenter& center, Module& module) override;
};

class CargoContainer : public ExternalObject
{
public:
    explicit CargoContainer(int strength = 50);
    void interact(CommandCenter& center, Module& module) override;
};

class SpaceAnomaly : public ExternalObject
{
public:
    explicit SpaceAnomaly(int strength = 10);
    void interact(CommandCenter& center, Module& module) override;   

    void interactWithEffect(CommandCenter& center, Module& module, int effect);
};
