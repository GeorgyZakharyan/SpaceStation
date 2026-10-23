#pragma once

class Durability 
{
private:
    int value_;     
    int maxValue_;  

public:
    Durability();                       
    Durability(int value);              
    Durability(int value, int maxValue);

    int getValue() const;
    int getMax() const;
    void setValue(int newValue);        

    void decrease(int amount);          
    void repair(int amount);            
};
