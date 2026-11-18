#include "IObservable.h"

IObservable::IObservable() {}

IObservable::IObservable(const IObservable&) {}

IObservable& IObservable::operator=(const IObservable&)
{
    return *this;
}

IObservable::~IObservable() {}

void IObservable::subscribe(IObserver* observer)
{
    if (observer == nullptr)
    {
        return;
    }
    for (size_t i = 0; i < observers_.size(); i++)
    {
        if (observers_[i] == observer)
        {
            return;     
        }
    }
    observers_.push_back(observer);
}

void IObservable::unsubscribe(IObserver* observer)
{
    for (size_t i = 0; i < observers_.size(); i++)
    {
        if (observers_[i] == observer)
        {
            observers_.erase(observers_.begin() + i);
            return;
        }
    }
}

void IObservable::notify(const ModuleEvent& event)
{
    for (size_t i = 0; i < observers_.size(); i++)
    {
        observers_[i]->onModuleEvent(event);
    }
}
