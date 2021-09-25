//epoll的封装

// typedef union epoll_data {
//     void        *ptr;
//     int          fd;
//     uint32_t     u32;
//     uint64_t     u64;
// } epoll_data_t;

// struct epoll_event {
//     uint32_t     events;      /* Epoll events */  需要监听的事件
//     epoll_data_t data;        /* User data variable */ 需要监听的文件描述符
// };

#ifndef _POLLER_H_
#define _POLLER_H_

#include "Channel.h"
#include <mutex>
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>

class Poller
{
private:
    typedef std::vector<Channel*> ChannelList;
    /* data */
    //epoll实例
    int epfd_;

    //epoll_wait的事件数组参数，接收就绪事件
    std::vector<struct epoll_event> readyevents_;

    //事件表
    std::unordered_map<int, Channel*> channelmap_;

    //事件表的互斥操作
    std::mutex mapmutex_;

public:
    Poller(/* args */);
    ~Poller();

    //epoll_ctl()
    //添加事件
    void addChannel(Channel* channel);

    //删除事件
    void removeChannel(Channel* channel);

    //修改事件
    void updateChannel(Channel* channel);

    //监听事件，epoll_wait()
    void waitChannel(ChannelList& activeChannels);
};


#endif