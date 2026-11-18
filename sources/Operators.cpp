#include "Operators.h"
#include "CommandCenter.h"
#include "Module.h"
#include "ResourcePackage.h"
#include "Damage.h"

CommandCenter& operator+(CommandCenter& center, const ResourcePackage& package)
{
    center.addResources(package);
    return center;
}

Module& operator-(Module& module, const Damage& damage)
{
    module.takeDamage(damage.getAmount());
    return module;
}

Module& operator*(Module& module, double modifier)
{
    int newEfficiency = (int)(module.getEfficiency() * modifier + 0.5);
    module.setEfficiency(newEfficiency);
    return module;
}
