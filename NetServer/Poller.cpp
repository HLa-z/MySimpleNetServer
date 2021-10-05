//epoll_ctl(int epfd, int op, int fd, struct epoll_event* event);
//op : **EPOLL_CTL_ADD 添加,  EPOLL_CTL_DEL 删除, EPOLL_CTL_MOD 修改**

//int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);


#include "Poller.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <iostream>

#define TIMEOUT 1000
#define MAXEVENNUM 4096

Poller::Poller():
    epfd_(-1), 
    readyevents_(MAXEVENNUM),
    channelmap_(),
    mapmutex_()
{
    epfd_ = epoll_create(1);
    if(epfd_ <= 0){
        perror("epoll_create error");
        exit(-1);
    }
}

Poller::~Poller(){
    close(epfd_);
}

void Poller::addChannel(Channel* channel){
    int fd = channel->getFd();
    struct epoll_event event;
    event.events = channel->getEvents();
    event.data.ptr = channel;

    if(epoll_ctl(epfd_, EPOLL_CTL_ADD, fd, &event) < 0){
        perror("addChannel error");
        exit(-1);
    }
    {
        std::lock_guard<std::mutex> lock(mapmutex_);
        channelmap_[fd] = channel;
    }
    //std::cout << "addChannel : " << fd << std::endl;
}

void Poller::removeChannel(Channel* channel){
    int fd = channel->getFd();
    struct epoll_event event;
    event.events = channel->getEvents();
    event.data.ptr = channel;

    if(epoll_ctl(epfd_, EPOLL_CTL_DEL, fd, &event) < 0){
        perror("removeChannel error");
        exit(-1);
    }
    //std::cout << "removeChannel : " << fd << std::endl;    
}

void Poller::updateChannel(Channel* channel){
    int fd = channel->getFd();
    struct epoll_event event;
    event.events = channel->getEvents();
    event.data.ptr = channel;

    if(epoll_ctl(epfd_, EPOLL_CTL_MOD, fd, &event) < 0){
        perror("updateChannel error");
        exit(-1);
    }
    //std::cout << "updateChannel : " << fd << std::endl;    
}

//int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
void Poller::waitChannel(ChannelList& activeChannels){
    int nfds = epoll_wait(epfd_, &readyevents_[0], readyevents_.capacity(), TIMEOUT);
    if(nfds == -1){
        perror("epoll_wait error");
        return ;
    }
    //扩容
    if(nfds >= readyevents_.capacity()){
        readyevents_.resize(2 * nfds);
    }
    for(int i = 0; i < nfds; ++i){
        uint32_t events = readyevents_[i].events;
        Channel* channel = (Channel*)readyevents_[i].data.ptr;
        int fd = channel->getFd();
        int exitfd = 0;
        {
            std::lock_guard<std::mutex> lock(mapmutex_);
            exitfd = channelmap_.count(fd);
        }
        if(exitfd != 0){
            channel->setEvents(events);
            activeChannels.push_back(channel);
        }
        else{
            std::cout << "channel not find" << std::endl;
        }
    }
}