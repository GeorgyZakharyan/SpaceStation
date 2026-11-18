#pragma once
#include <vector>
#include "IObserver.h"

class IObservable
{
private:
    std::vector<IObserver*> observers_;

public:
    IObservable();
    IObservable(const IObservable& other);              
    IObservable& operator=(const IObservable& other);   
    virtual ~IObservable();

    void subscribe(IObserver* observer);
    void unsubscribe(IObserver* observer);
    void notify(const ModuleEvent& event);
};
