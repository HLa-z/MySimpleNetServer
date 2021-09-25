//事件循环线程池
#ifndef _EVENTLOOPTHREADPOOL_H_
#define _EVENTLOOPTHREADPOOL_H_

#include "EventLoopThread.h"
#include <vector>

class EventLoopThreadPool
{
private:
    typedef std::vector<EventLoopThread*> EVLoopThreadList;
    /* data */

    //如果线程池没有IO线程，就返回主loop
    EventLoop* mainloop_;

    //线程池
    EVLoopThreadList pool_;

    //轮询顺序获取evloop
    int index_;

    //线程数量
    int threadnum_;

public:
    EventLoopThreadPool(EventLoop* mainloop, int threadnum = 0);
    ~EventLoopThreadPool();

    EventLoop* getNextEventLoop();

    void start();
};

#endif