//事件循环，通过调用Poller监听，然后分发事件

#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include "Poller.h"
#include "Channel.h"
#include <mutex>
#include <vector>
#include <functional>

class EventLoop
{
private:
    typedef std::function<void()> Function;
    typedef std::vector<Channel*> ChannelList;

    /* data */
    Poller poller_;

    //停止循环
    bool stop_;

    //任务列表
    std::vector<Function> tasklist_;

    //任务列表锁
    std::mutex tasklistmutex_;

    int wakeupfd_;
    //唤醒evenloop通道
    //为什么要唤醒？因为在循环监听过程会阻塞在poller的waitChannel，而
    Channel wakeupchannel_;

    //传给poller.waitChannel接收有事件的channel
    ChannelList activechannel_;

public:
    EventLoop(/* args */);
    ~EventLoop();

    void loop();

    void stop();

    //添加任务
    void addTask(Function task);

    //执行任务列表的任务
    void executeAllTask();

    //唤醒
    void wakeUp();

    //唤醒后触发的事件
    void handleRead();  

    //添加事件
    void addChannelToPoller(Channel* channel);  

    void removeChannelToPoller(Channel* channel);  

    void updateChannelToPoller(Channel* channel);  
};


#endif