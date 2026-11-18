#pragma once
#include <string>

class Damage
{
private:
    int amount_;
    std::string type_;

public:
    Damage(int amount, const std::string& type);

    int getAmount() const;
    const std::string& getType() const;
};
