#include <iostream>
#include <algorithm>
#include "Station.h"

StationIterator::StationIterator(ModuleVector::iterator current, ModuleVector::iterator end): current_(current), end_(end) {}

Module& StationIterator::operator*() const
{
    return **current_;
}

StationIterator& StationIterator::operator++()
{
    if (current_ != end_)
    {
        ++current_;
    }
    return *this;
}

bool StationIterator::operator!=(const StationIterator& other) const
{
    return current_ != other.current_;
}

bool StationIterator::operator==(const StationIterator& other) const
{
    return current_ == other.current_;
}

ConstStationIterator::ConstStationIterator(ModuleVector::const_iterator current, ModuleVector::const_iterator end)
    : current_(current), end_(end) {}

const Module& ConstStationIterator::operator*() const
{
    return **current_;
}

ConstStationIterator& ConstStationIterator::operator++()
{
    if (current_ != end_)
    {
        ++current_;
    }
    return *this;
}

bool ConstStationIterator::operator!=(const ConstStationIterator& other) const
{
    return current_ != other.current_;
}

bool ConstStationIterator::operator==(const ConstStationIterator& other) const
{
    return current_ == other.current_;
}

Station::Station(size_t maxModules) : Station("Станция", maxModules) {}

Station::Station(const std::string& name, size_t maxModules) : name_(name), maxModules_(maxModules < 1 ? 1 : maxModules) {}

Station::Station(const Station& other) : name_(other.name_), maxModules_(other.maxModules_)
{
    for (size_t i = 0; i < other.modules_.size(); i++)
        modules_.push_back(other.modules_[i]->clone());
}

Station& Station::operator=(const Station& other)
{
    if (this == &other)
    {
        return *this;
    }
    name_ = other.name_;
    maxModules_ = other.maxModules_;
    modules_.clear();
    for (size_t i = 0; i < other.modules_.size(); i++)
    {
        modules_.push_back(other.modules_[i]->clone());
    }
    return *this;
}

Station::Station(Station&& other) noexcept : name_(std::move(other.name_)), maxModules_(other.maxModules_),
      modules_(std::move(other.modules_))
{
    other.maxModules_ = 0;
    other.modules_.clear();
}

Station& Station::operator=(Station&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }
    name_ = std::move(other.name_);
    maxModules_ = other.maxModules_;
    modules_ = std::move(other.modules_);
    other.maxModules_ = 0;
    other.modules_.clear();
    return *this;
}

void Station::updatePositions()
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        modules_[i]->moveToIndex((int)i);    
    }
}

bool Station::addModule(std::unique_ptr<Module> module)
{
    if (module == nullptr)
    {
        return false;
    }
    if (modules_.size() >= maxModules_)
    {
        std::cout << "Нельзя добавить " << module->getName()
                  << ": станция заполнена (максимум " << maxModules_ << ")" << std::endl;
        return false;
    }
    modules_.push_back(std::move(module));
    updatePositions();
    return true;
}

bool Station::removeByIndex(size_t index)       
{
    if (index >= modules_.size())
    {
        std::cout << "Неверный индекс: " << index << std::endl;
        return false;
    }
    modules_.erase(modules_.begin() + index);
    updatePositions();
    return true;
}

bool Station::removeById(int id)
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getId() == id)
        {
            modules_.erase(modules_.begin() + i);
            updatePositions();
            return true;
        }
    }
    std::cout << "Модуль с id=" << id << " не найден" << std::endl;
    return false;
}

bool Station::moveModule(size_t fromIndex, size_t toIndex)      
{
    size_t n = modules_.size();
    if (fromIndex >= n || toIndex >= n)
    {
        std::cout << "Неверные индексы для перемещения" << std::endl;
        return false;
    }
    if (fromIndex == toIndex)
    {
        return true;
    }
    std::unique_ptr<Module> temp = std::move(modules_[fromIndex]);
    modules_.erase(modules_.begin() + fromIndex);
    modules_.insert(modules_.begin() + toIndex, std::move(temp));
    updatePositions();
    return true;
}

int Station::getCount() const
{
    return (int)modules_.size();
}

size_t Station::getMax() const
{
    return maxModules_;
}

const std::string& Station::getName() const
{
    return name_;
}

Module* Station::getModule(int index) const
{
    if (index < 0 || index >= (int)modules_.size())
    {
        return nullptr;
    }
    return modules_[index].get();
}

Module* Station::findById(int id) const
{
    for (size_t i = 0; i < modules_.size(); i++)
    {
        if (modules_[i]->getId() == id)
        {
            return modules_[i].get();
        }
    }
    return nullptr;
}

void Station::sortByEfficiency()
{
    std::stable_sort(modules_.begin(), modules_.end(),
        [](const std::unique_ptr<Module>& a, const std::unique_ptr<Module>& b)
        {
            return a->getEfficiency() > b->getEfficiency();
        });
    updatePositions();
}

void Station::showAll() const
{
    std::cout << "Станция \"" << name_ << "\" (" << modules_.size()
              << "/" << maxModules_ << " модулей)" << std::endl;
    for (ConstStationIterator it = begin(); it != end(); ++it)
    {
        std::cout << "  ";
        (*it).showInfo();
    }
}

StationIterator Station::getIterator()
{
    return begin();
}

ConstStationIterator Station::getIterator() const
{
    return begin();
}

StationIterator Station::begin()
{
    return StationIterator(modules_.begin(), modules_.end());
}

StationIterator Station::end()
{
    return StationIterator(modules_.end(), modules_.end());
}

ConstStationIterator Station::begin() const
{
    return ConstStationIterator(modules_.begin(), modules_.end());
}

ConstStationIterator Station::end() const
{
    return ConstStationIterator(modules_.end(), modules_.end());
}