
#ifndef _LOGFILE_H_
#define _LOGFILE_H

#include "File.h"
#include <mutex>

class LogFile
{
private:
    /* data */
    File file_;
    std::mutex mutex_;
    int count_;
    const int flushN_;

public:
    LogFile(const std::string& filename, int flushN = 3);
    ~LogFile();

    void append(const char* logline, int len);
    void flush();
};


#endif