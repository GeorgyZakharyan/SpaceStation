#include "ResourcePackage.h"

ResourcePackage::ResourcePackage(int energy, int oxygen, int safetyLevel)
    : energy_(energy), oxygen_(oxygen), safetyLevel_(safetyLevel) {}

int ResourcePackage::getEnergy() const
{
    return energy_;
}

int ResourcePackage::getOxygen() const
{
    return oxygen_;
}

int ResourcePackage::getSafetyLevel() const
{
    return safetyLevel_;
}
