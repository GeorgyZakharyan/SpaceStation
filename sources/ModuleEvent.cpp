#include "ModuleEvent.h"

int ModuleEvent::nextTimestamp_ = 1;

ModuleEvent::ModuleEvent(EventType type, int moduleId, int damage)
    : type_(type), moduleId_(moduleId), damage_(damage), timestamp_(nextTimestamp_++) {}

EventType ModuleEvent::getType() const
{
    return type_;
}

int ModuleEvent::getModuleId() const
{
    return moduleId_;
}

int ModuleEvent::getDamage() const
{
    return damage_;
}

int ModuleEvent::getTimestamp() const
{
    return timestamp_;
}

std::string ModuleEvent::getTypeName() const
{
    switch (type_)
    {
        case EVENT_ACTIVATED:
            return "активация";
        case EVENT_DAMAGED:
            return "повреждение";
        case EVENT_DESTROYED:
            return "разрушение";
        case EVENT_ENERGY_CONSUMED:
            return "расход энергии";
    }
    return "неизвестно";
}
