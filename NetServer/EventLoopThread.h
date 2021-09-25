
#ifndef _EVENTLOOPTHREAD_H_
#define _EVENTLOOPTHREAD_H_

#include "EventLoop.h"
#include <thread>

class EventLoopThread
{
private:
    /* data */
    EventLoop* evloop_;

    std::thread thread_;

    std::thread::id id_;

public:
    EventLoopThread();
    ~EventLoopThread();

    void start();

    void threadFunc();

    EventLoop* getEventLoop() { return evloop_; }
};



#endif