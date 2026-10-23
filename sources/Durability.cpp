#include "Durability.h"

Durability::Durability(): value_(100), maxValue_(100) {}

Durability::Durability(int value): value_(0), maxValue_(100) 
{
    setValue(value);
}

Durability::Durability(int value, int maxValue): value_(0), maxValue_(maxValue < 1 ? 1 : maxValue)
{
    setValue(value);
}

int Durability::getValue() const 
{
    return value_;
}

int Durability::getMax() const 
{
    return maxValue_;
}

void Durability::setValue(int newValue) 
{
    if (newValue < 0) 
    {
        value_ = 0;
    } else if (newValue > maxValue_) 
    {
        value_ = maxValue_;
    } else 
    {
        value_ = newValue;
    }
}

void Durability::decrease(int amount) 
{
    if (amount < 0) 
    {
        return;
    }
    value_ = value_ - amount;
    if (value_ < 0) 
    {
        value_ = 0;
    }
}

void Durability::repair(int amount) 
{
    if (amount < 0) 
    {
        return;
    }
    value_ = value_ + amount;
    if (value_ > maxValue_) 
    {
        value_ = maxValue_;
    }
}
