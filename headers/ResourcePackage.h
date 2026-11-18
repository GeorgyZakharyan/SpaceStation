#pragma once

class ResourcePackage
{
private:
    int energy_;
    int oxygen_;
    int safetyLevel_;

public:
    ResourcePackage(int energy = 0, int oxygen = 0, int safetyLevel = 0);

    int getEnergy() const;
    int getOxygen() const;
    int getSafetyLevel() const;
};
