#ifndef NARUTO_LOG_APPENDER_INTERFACE_H
#define NARUTO_LOG_APPENDER_INTERFACE_H

#include <memory>

namespace naruto
{
class LogAppenderInterface{
public:
    using Ptr = std::shared_ptr<LogAppenderInterface>;

public:
    virtual ~LogAppenderInterface(){};
    virtual void append(const char* msg, size_t len) = 0;
};
}


#endif
