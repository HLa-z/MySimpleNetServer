
#include "LogFile.h"

LogFile::LogFile(const std::string& filename, int flushN):
    file_(filename),
    mutex_(),
    flushN_(flushN),
    count_(0)
{

}

LogFile::~LogFile(){}

void LogFile::append(const char* logline, int len){
    std::lock_guard<std::mutex> lock(mutex_);
    ++count_;
    file_.append(logline, len);
    if(count_ >= flushN_){
        this->flush();
        count_ = 0;
    }
}

void LogFile::flush(){
    file_.flush();
}