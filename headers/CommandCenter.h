#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cstddef>
#include "Module.h"
#include "IObserver.h"
#include "Environment.h"
#include "EnvironmentProxy.h"
#include "ResourcePackage.h"

class ModuleFactory;
class ExternalObject;

class CommandCenter : public IObserver
{
private:
    std::string name_;
    int energy_;                
    int oxygen_;                
    int safetyLevel_;           
    int radiation_;             
    size_t maxModules_;
    std::vector<std::unique_ptr<Module> > modules_;
    std::shared_ptr<EnvironmentProxy> currentEnvironment_;   
    std::vector<int> destroyedIds_;                          
    int eventCount_;            

    void updatePositions();
    void setSafetyLevel(int newLevel);
    void cloneModulesFrom(const CommandCenter& other);

public:
    CommandCenter(const std::string& name, size_t maxModules, int energy, int oxygen, int safetyLevel);
    ~CommandCenter() = default;
    CommandCenter(const CommandCenter& other);
    CommandCenter& operator=(const CommandCenter& other);
    CommandCenter(CommandCenter&& other);
    CommandCenter& operator=(CommandCenter&& other);

    bool addModule(std::unique_ptr<Module> module);
    bool createResidentialModule(ModuleFactory& factory, const std::string& name, int variant = 0);
    bool createEnergyModule(ModuleFactory& factory, const std::string& name, int variant = 0);
    bool createResearchModule(ModuleFactory& factory, const std::string& name, int variant = 0);
    bool removeById(int id);
    void removeDestroyedModules();
    Module* findById(int id) const;
    Module* getModule(int index) const;
    int getModuleCount() const;
    bool teleportModule(int id);
    bool transferEnergy(int fromId, int toId, int amount);

    const std::string& getName() const;
    int getEnergy() const;
    int getOxygen() const;
    int getSafetyLevel() const;
    int getRadiation() const;
    size_t getMaxModules() const;
    int getEventCount() const;
    void addResources(const ResourcePackage& package);
    void addRadiation(int amount);

    void setEnvironment(const std::shared_ptr<Environment>& environment);
    std::string getEnvironmentName() const;
    double getEnergyModifier() const;
    double getDamageModifier() const;
    int calculateDamage(int baseAmount) const;      
    void applyEnvironment();
    std::shared_ptr<EnvironmentProxy> getEnvironmentProxy() const;

    void tick();

    bool receiveObject(ExternalObject& object, int moduleId);
    
    void onModuleEvent(const ModuleEvent& event) override;

    void showStatistics() const;
};
