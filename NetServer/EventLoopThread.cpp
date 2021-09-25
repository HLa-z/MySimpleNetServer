#include "EventLoopThread.h"
#include <iostream>

EventLoopThread::EventLoopThread():
    evloop_(nullptr),
    id_(-1),
    thread_()
{

}

EventLoopThread::~EventLoopThread(){
    evloop_->stop();
    thread_.join();
    std::cout << "EventLoopThread " << id_ << " end" << std::endl;
}

void EventLoopThread::start(){
    //std::cout << "EventLoopThread::start" << std::endl;
    thread_ = std::thread(&EventLoopThread::threadFunc, this);
}

void EventLoopThread::threadFunc(){
    EventLoop evloop;
    evloop_ = &evloop;
    id_ = thread_.get_id();
    std::cout << "EventLoopThread " << id_ << " start" << std::endl;
    evloop_->loop();
}