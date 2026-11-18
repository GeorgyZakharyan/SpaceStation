#pragma once
#include <string>

// Типы событий, о которых модуль сообщает командному центру
enum EventType
{
    EVENT_ACTIVATED,        // модуль включили
    EVENT_DAMAGED,          // модуль получил урон
    EVENT_DESTROYED,        // прочность модуля упала до нуля
    EVENT_ENERGY_CONSUMED   // модуль потратил энергию
};

class ModuleEvent
{
private:
    EventType type_;
    int moduleId_;
    int damage_;        
    int timestamp_;     

    static int nextTimestamp_;

public:
    ModuleEvent(EventType type, int moduleId, int damage = 0);

    EventType getType() const;
    int getModuleId() const;
    int getDamage() const;
    int getTimestamp() const;
    std::string getTypeName() const;
};
