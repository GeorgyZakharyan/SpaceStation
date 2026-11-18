#pragma once
#include <string>

enum class LogSource
{
    ENV,       
    OBJECT,    
    MODULE,    
    CENTER     
};

class LoggingProxy
{
private:
    static int count_;
    static int sourceCounts_[4];        
    static bool enabled_;

public:
    static void log(LogSource source, const std::string& message);
    static std::string getSourceName(LogSource source);

    static int getCount();
    static int getCount(LogSource source);
    static void setEnabled(bool enabled);
};
