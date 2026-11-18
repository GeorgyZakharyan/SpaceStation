#include "Damage.h"

Damage::Damage(int amount, const std::string& type) : amount_(amount < 0 ? 0 : amount), type_(type) {}

int Damage::getAmount() const
{
    return amount_;
}

const std::string& Damage::getType() const
{
    return type_;
}
