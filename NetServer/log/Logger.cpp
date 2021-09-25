
#include "Logger.h"
#include "AsyncLogging.h"
#include <time.h>

const char* loglevelstr[Logger::LOG_LEVEL_NUM] = 
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR",
};

static std::once_flag oflag;
static std::shared_ptr<AsyncLogging> spAsyncLogging;
static std::string logfilename = "./NetServer.log";

void init(){
    spAsyncLogging = std::make_shared<AsyncLogging>(logfilename, 30);
    spAsyncLogging->start();
}

void output(const char* logline, int len){
    std::call_once(oflag, init);
    spAsyncLogging->append(logline, len);
}

Logger::Logger(const char* filename, int line, LogLevel level):
    stream_(),
    filename_(filename),
    line_(line),
    level_(level)
{
    formatLogHead();
}

Logger::~Logger(){
    stream_ << "-- [file:" << filename_ << ",line:" << line_ << "]\n";
    const LogStream::Buffer& buff = stream_.getBuffer();
    output(buff.getData(), buff.length());
}

void Logger::formatLogHead(){
    time_t rawtime;
    struct tm *info;
    char time_str[30];
    time(&rawtime);
    info = localtime(&rawtime);
    //strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", info);
    strftime(time_str, 30, "[%Y-%m-%d %H:%M:%S] ", info);
    stream_ << time_str;
    stream_ << '[' << loglevelstr[level_] << "] --";
}

