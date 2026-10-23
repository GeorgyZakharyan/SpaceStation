#include "Capacity.h"

Capacity::Capacity(): value_(0) {}

Capacity::Capacity(int value): value_(value < 0 ? 0 : value) {}

int Capacity::getValue() const 
{
    return value_;
}

void Capacity::setValue(int newValue) 
{
    if (newValue < 0) 
    {
        newValue = 0;
    }
    value_ = newValue;
}
