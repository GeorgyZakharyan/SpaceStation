#pragma once

class EnergyConsumption 
{
private:
    int value_;

public:
    EnergyConsumption();
    EnergyConsumption(int value);

    int getValue() const;
    void setValue(int newValue);

    void increase(int amount);
    void decrease(int amount);
};
