
#ifndef _ASYNCLOG_H_
#define _ASYNCLOG_H_

#include "LogStream.h"
#include <memory>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>

class AsyncLogging
{
private:
    typedef FixBuffer<LARGESIZE> Buffer;
    typedef std::unique_ptr<Buffer> BufferPtr;
    typedef std::vector<BufferPtr> BufferVector; 
    /* data */
    //当前缓冲区
    BufferPtr currBuffer_;
    //备用缓冲区
    BufferPtr nextBuffer_;

    //待写入文件的已满缓冲区
    BufferVector fullBuffers_;
    std::mutex mutex_;
    std::condition_variable cv_;

    //每个那么多时间将缓冲区日志刷新到文件
    const int flushInterval_;
    bool running_;

    std::thread thread_;
    std::string logfilename_;

    void threadFunc();

public:
    AsyncLogging(const std::string& logfilename, int flushInterval);
    ~AsyncLogging();

    void start();
    void stop();
    void append(const char* logline, int len);

};

#endif