/*
* Channel类
* 封装fd和相应事件处理
*/

#ifndef _CHANNEL_H_
#define _CHANNEL_H_

#include <functional>

class Channel
{
private:
    typedef std::function<void()> Callback;

    /* data */
    //文件描述符
    int fd_;

    //事件类型
    uint32_t events_;

    //事件触发的回调函数
    Callback read_handler_;     //读事件触发
    Callback write_handler_;    //写事件触发
    Callback error_handler_;    //错误触发
    Callback close_handler_;    //关闭触发

public:
    Channel(/* args */);
    ~Channel();

    int getFd() const{ return fd_; }

    void setFd(int fd){ fd_ = fd; }

    uint32_t getEvents() const{ return events_; }

    //
    void setEvents(uint32_t events){
        events_ = events;
    }

    //设置事件回调
    void setReadHandler(const Callback& cb){
        read_handler_ = cb;
    }

    void setWriteHandler(const Callback& cb){
        error_handler_ = cb;
    }

    void setErrorHandler(const Callback& cb){
        error_handler_ = cb;
    }

    void setCloseHandler(const Callback& cb){
        close_handler_ = cb;
    }

    //事件触发时选择调用函数
    void handleEvent();
};

#endif