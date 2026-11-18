#include "ExternalObject.h"

ExternalObject::ExternalObject(const std::string& type, int strength)
    : type_(type), strength_(strength < 0 ? 0 : strength) {}

ExternalObject::~ExternalObject() {}

const std::string& ExternalObject::getType() const
{
    return type_;
}

int ExternalObject::getStrength() const
{
    return strength_;
}
