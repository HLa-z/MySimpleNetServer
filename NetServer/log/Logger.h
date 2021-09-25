
#ifndef _LOGGER_H_
#define _LOGGER_H_

#include "LogStream.h"

class Logger
{
public:
    enum LogLevel{
        DEBUG,
        INFO,
        WARN,
        ERROR,
        LOG_LEVEL_NUM
    };

private:
    /* data */
    LogStream stream_;
    std::string filename_;
    int line_;
    LogLevel level_;
    static std::string logfilename_;

    void formatLogHead();

public:
    Logger(const char* filename, int line, LogLevel level);
    ~Logger();

    LogStream& Stream() { return stream_; }
    static void setLogFileName(const std::string& logfilename) { logfilename_ = logfilename; }
};

#define LOG_DEBUG Logger(__FILE__, __LINE__, Logger::DEBUG).Stream()
#define LOG_INFO Logger(__FILE__, __LINE__, Logger::INFO).Stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).Stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).Stream()

#endif