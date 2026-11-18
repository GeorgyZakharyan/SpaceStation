#include <iostream>
#include "LoggingProxy.h"

int LoggingProxy::count_ = 0;
int LoggingProxy::sourceCounts_[4] = {0, 0, 0, 0};
bool LoggingProxy::enabled_ = true;

std::string LoggingProxy::getSourceName(LogSource source)
{
    switch (source)
    {
        case LogSource::ENV:
            return "ENV";
        case LogSource::OBJECT:
            return "OBJECT";
        case LogSource::MODULE:
            return "MODULE";
        case LogSource::CENTER:
            return "CENTER";
    }
    return "?";
}

void LoggingProxy::log(LogSource source, const std::string& message)
{
    count_++;
    sourceCounts_[(int)source]++;
    if (enabled_)
    {
        std::cout << "  [" << getSourceName(source) << "] " << message << std::endl;
    }
}

int LoggingProxy::getCount()
{
    return count_;
}

int LoggingProxy::getCount(LogSource source)
{
    return sourceCounts_[(int)source];
}

void LoggingProxy::setEnabled(bool enabled)
{
    enabled_ = enabled;
}
