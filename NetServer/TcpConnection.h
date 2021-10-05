
#ifndef _TCPCONNECTION_H_
#define _TCPCONNECTION_H_

#include "Channel.h"
#include "EventLoop.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>
#include <memory>
#include <functional>

#define BUFFSIZE 4096

//每次有新连接时，是通过TCPServer，new TcpConnection达到新建连接目的
//空间分配在堆上，所以用智能指针管理，
//类成员需要传递this，直接传this很危险，
//所以继承enable_shared_from_this类，方便传递一个共享同对象的shared_ptr
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
private:
    typedef std::shared_ptr<TcpConnection> TcpConnection_sptr;
    typedef std::function<void(const TcpConnection_sptr&)> Callback;
    typedef std::function<void(const TcpConnection_sptr&, std::string&)> MessageCallback; //接收消息回调
    /* data */
    //对方描述符
    int fd_;
    //对方地址
    struct sockaddr_in oppositeaddr_;

    Channel channel_;

    EventLoop* loop_;

    //连接关闭
    bool closed_;

    //输入输出缓冲区
    std::string bufferin_;
    std::string bufferout_;

    //回调函数
    MessageCallback msgCallback_;
    Callback writeFinishedCallback_;
    Callback closeTcpconnectionCallback_; //应用层的回调
    Callback errorCallback_;
    Callback clearConnection_;  //在tcpserver中删除连接

public:
    TcpConnection(int fd, EventLoop* loop, struct sockaddr_in oppositeaddr);
    ~TcpConnection();

    //绑定到Channel的回调函数
    void readHandle();
    void writeHandle();
    void closeHandle();
    void errorHandle();

    int getFd() const{ return fd_; }
    struct sockaddr_in getAddr() const{ return oppositeaddr_; }

    //把此连接的channel添加到loop监听
    void addChannelToEventLoop();

    //对上层提供的发送数据的接口，将data写入bufferout_
    void sendData(const std::string& data);

    //数据发送要在负责IO的loop中发送，更新loop中的的channel，设置envens为EPOLLOUT
    void sendByLoop();

    void shutdown();

    //设置回调函数
    void setMessageCallback(const MessageCallback& mcb){
        msgCallback_ = mcb;
    }
    void setCloseTcpconnection(const Callback& cb){
        closeTcpconnectionCallback_ = cb;
    }
    void setWriteFinishedCallback(const Callback& cb){
        writeFinishedCallback_ = cb;
    }
    void setErrorCallback_(const Callback& cb){
        errorCallback_ = cb;
    }
    void setClearConnetionCallback(const Callback& cb){
        clearConnection_ = cb;
    }
};


#endif