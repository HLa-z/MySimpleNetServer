
#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#include "Socket.h"
#include "EventLoop.h"
#include "Channel.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"
#include <map>

#define MAXCONNECTION 10000

class TcpServer
{
private:
    typedef std::shared_ptr<TcpConnection> TcpConnection_sptr;
    typedef std::function<void(const TcpConnection_sptr&)> Callback;
    typedef std::function<void(const TcpConnection_sptr&, std::string&)> MessageCallback;
    /* data */
    Socket socket_;

    EventLoop* mainloop_;

    Channel channel_;

    int conncount_;

    std::map<int, TcpConnection_sptr> connectionmap_;

    EventLoopThreadPool evloopthreadpool_;

    void createNewConnection();
    void removeConnection(const TcpConnection_sptr& tcpconnection);
    void errorConnection();

    //业务回调函数
    MessageCallback msgCallback_;
    Callback writeFinishedCallback_;
    Callback closeCallback_;
    Callback errorCallback_;

public:
    TcpServer(EventLoop* loop, const int& port, const int& iothreadnum);
    ~TcpServer();

    void start();

    //设置回调函数
    void setMessageCallback(const MessageCallback& mcb){
        msgCallback_ = mcb;
    }
    void setCloseTcpconnection(const Callback& cb){
        closeCallback_ = cb;
    }
    void setWriteFinishedCallback(const Callback& cb){
        writeFinishedCallback_ = cb;
    }
    void setErrorCallback_(const Callback& cb){
        errorCallback_ = cb;
    }
};


#endif