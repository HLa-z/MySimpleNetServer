//#include <sys/eventfd.h>
//int eventfd(unsigned int initval, int flags);

#include "EventLoop.h"
#include <unistd.h>
#include <iostream>
#include <sys/eventfd.h>

int getWakeUpFD(){
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(fd < 0){
        perror("eventfd() error");
        exit(-1);
    }
    std::cout << "getWakeUpFD" << std::endl;
    return fd;
}

EventLoop::EventLoop():
    poller_(),
    stop_(true),
    tasklist_(),
    tasklistmutex_(),
    wakeupfd_(-1),
    wakeupchannel_(),
    activechannel_() {
        wakeupfd_ = getWakeUpFD();
        wakeupchannel_.setFd(wakeupfd_);
        wakeupchannel_.setEvents(EPOLLIN | EPOLLET);
        wakeupchannel_.setReadHandler(std::bind(&EventLoop::handleRead, this));
        addChannelToPoller(&wakeupchannel_);
}

EventLoop::~EventLoop(){
    close(wakeupfd_);
}

void EventLoop::loop(){
    stop_ = false;
    while(!stop_){
        poller_.waitChannel(activechannel_);
        for(Channel* channle : activechannel_){
            channle->handleEvent();
        }
        //std::cout << "executeAllTask..." << std::endl;
        activechannel_.clear();
        executeAllTask();
    }
}

void EventLoop::stop(){
    stop_ = true;
}

void EventLoop::addTask(Function task){
    {
        std::lock_guard<std::mutex> lock(tasklistmutex_);
        tasklist_.push_back(task);
    }
    wakeUp();
}

void EventLoop::executeAllTask(){
    //执行过程中，tasklist可能会改变，装到别的地方再执行
    std::vector<Function> temp;
    {
        std::lock_guard<std::mutex> lock(tasklistmutex_);
        temp.swap(tasklist_);
    }
    for(Function& func : temp){
        func();
    }
    temp.clear();
}

void EventLoop::wakeUp(){
    uint64_t on = 1;
    write(wakeupfd_, &on, sizeof(on));
}

void EventLoop::handleRead(){
    uint64_t on = 0;
    read(wakeupfd_, &on, sizeof(on));
}

void EventLoop::addChannelToPoller(Channel* channel){
    poller_.addChannel(channel);
}

void EventLoop::updateChannelToPoller(Channel* channel){
    poller_.updateChannel(channel);
}

void EventLoop::removeChannelToPoller(Channel* channel){
    poller_.removeChannel(channel);
}