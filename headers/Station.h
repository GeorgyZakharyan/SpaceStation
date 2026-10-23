#pragma once
#include <vector>
#include <memory>
#include <string>
#include <cstddef>
#include "Module.h"

typedef std::vector<std::unique_ptr<Module> > ModuleVector;

class StationIterator
{
private:
    ModuleVector::iterator current_;
    ModuleVector::iterator end_;

public:
    StationIterator(ModuleVector::iterator current, ModuleVector::iterator end);

    Module& operator*() const;
    StationIterator& operator++();
    bool operator!=(const StationIterator& other) const;
    bool operator==(const StationIterator& other) const;
};

class ConstStationIterator
{
private:
    ModuleVector::const_iterator current_;
    ModuleVector::const_iterator end_;

public:
    ConstStationIterator(ModuleVector::const_iterator current, ModuleVector::const_iterator end);

    const Module& operator*() const;
    ConstStationIterator& operator++();
    bool operator!=(const ConstStationIterator& other) const;
    bool operator==(const ConstStationIterator& other) const;
};

class Station
{
private:
    std::string name_;
    size_t maxModules_;
    ModuleVector modules_;

    void updatePositions();

public:
    explicit Station(size_t maxModules);                       
    Station(const std::string& name, size_t maxModules);

    ~Station() = default;                                       
    Station(const Station& other);
    Station& operator=(const Station& other);
    Station(Station&& other) noexcept;
    Station& operator=(Station&& other) noexcept;

    bool addModule(std::unique_ptr<Module> module);
    bool removeByIndex(size_t index);                          
    bool removeById(int id);
    bool moveModule(size_t fromIndex, size_t toIndex);          

    int getCount() const;
    size_t getMax() const;
    const std::string& getName() const;
    Module* getModule(int index) const;
    Module* findById(int id) const;

    void sortByEfficiency();                                    

    void showAll() const;

    StationIterator getIterator();
    ConstStationIterator getIterator() const;
    StationIterator begin();
    StationIterator end();
    ConstStationIterator begin() const;
    ConstStationIterator end() const;
};