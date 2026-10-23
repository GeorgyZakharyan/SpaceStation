#pragma once

class Capacity 
{
private:
    int value_;

public:
    Capacity();
    Capacity(int value);

    int getValue() const;
    void setValue(int newValue);
};


