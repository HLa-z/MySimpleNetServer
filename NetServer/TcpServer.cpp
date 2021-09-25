
#include "TcpServer.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

void setFdNonBlock(int& fd);

TcpServer::TcpServer(EventLoop* loop, const int& port, const int& iothreadnum):
    mainloop_(loop),
    socket_(),
    channel_(),
    conncount_(0),
    connectionmap_(),
    evloopthreadpool_(loop, iothreadnum)
{
    socket_.setAddrReuse();
    socket_.setNoBlock();
    socket_.Bind(port);
    socket_.Listen();

    channel_.setFd(socket_.getfd());
    channel_.setEvents(EPOLLIN | EPOLLET);
    channel_.setReadHandler(std::bind(&TcpServer::createNewConnection, this));
    channel_.setErrorHandler(std::bind(&TcpServer::errorConnection, this));
    channel_.setCloseHandler(std::bind(&TcpServer::errorConnection, this));
}

TcpServer::~TcpServer(){}

void TcpServer::createNewConnection(){
    //调用accept接收新连接，配置新的tcpconnection
    int connfd;
    struct sockaddr_in clientaddr;
    //ET模式，循环读到空
    while(true){
        connfd = socket_.Accept(clientaddr);
        if(connfd < 0){
            break;
        }
        if(conncount_ >= MAXCONNECTION){
            close(connfd);
            continue;
        }
        //std::cout << "connfd: " << connfd << std::endl;
        setFdNonBlock(connfd);

        EventLoop* evloop = evloopthreadpool_.getNextEventLoop();

        TcpConnection_sptr spconnection = std::make_shared<TcpConnection>(connfd, evloop, clientaddr);
        spconnection->setMessageCallback(msgCallback_);
        spconnection->setWriteFinishedCallback(writeFinishedCallback_);
        spconnection->setCloseTcpconnection(closeCallback_);
        spconnection->setErrorCallback_(errorCallback_);
        spconnection->setClearConnetionCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));

        ++conncount_;
        connectionmap_[connfd] = spconnection;

        spconnection->addChannelToEventLoop();
        std::cout << "New Connection --ip : " << inet_ntoa(clientaddr.sin_addr) << std::endl;
    }
}

void TcpServer::start(){
    evloopthreadpool_.start();
    mainloop_->addChannelToPoller(&channel_);
}

void TcpServer::errorConnection(){
    std::cout << "errorConnection" << std::endl;
    socket_.Close();
}

void TcpServer::removeConnection(const TcpConnection_sptr& tcpconnection){
    connectionmap_.erase(tcpconnection->getFd());
    --conncount_;
}

void setFdNonBlock(int& fd){
    int flag = fcntl(fd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flag);
}