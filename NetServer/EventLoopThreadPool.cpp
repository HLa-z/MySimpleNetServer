
#include "EventLoopThreadPool.h"
#include <iostream>

EventLoopThreadPool::EventLoopThreadPool(EventLoop* mainloop, int threadnum):
    mainloop_(mainloop),
    threadnum_(threadnum),
    pool_(),
    index_(0)
{
    for(int i = 0; i < threadnum_; ++i){
        EventLoopThread* evlt = new EventLoopThread();
        pool_.push_back(evlt);
    }
}

EventLoopThreadPool::~EventLoopThreadPool(){
    for(int i = 0; i < threadnum_; ++i){
        delete pool_[i];
    }
    std::cout << "EventLoopThreadPool end" << std::endl;
}

EventLoop* EventLoopThreadPool::getNextEventLoop(){
    if(threadnum_ == 0){
        return mainloop_;
    }
    else{
        //接收由主线程loop完成，分配监听事件不会产生竞争，不家所
        //std::lock_guard<std::mutex> lock(mutex_);
        index_ = (index_ + 1) % threadnum_;
        return pool_[index_]->getEventLoop();
    }
}

void EventLoopThreadPool::start(){
    for(int i = 0; i < threadnum_; ++i){
        pool_[i]->start();
    }
}