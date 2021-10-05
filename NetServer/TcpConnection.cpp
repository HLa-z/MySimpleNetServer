
#include "TcpConnection.h"
#include <unistd.h>
#include <iostream>

int readData(int fd, std::string& bufferin_);
int writeData(int fd, std::string& buffout_);

TcpConnection::TcpConnection(int fd, EventLoop* loop, struct sockaddr_in oppositeaddr):
    fd_(fd),
    oppositeaddr_(oppositeaddr),
    channel_(),
    loop_(loop),
    closed_(false),
    bufferin_(),
    bufferout_()
{
    channel_.setFd(fd_);
    channel_.setEvents(EPOLLIN | EPOLLET);
    channel_.setReadHandler(std::bind(&TcpConnection::readHandle, this));
    channel_.setWriteHandler(std::bind(&TcpConnection::writeHandle, this));
    channel_.setErrorHandler(std::bind(&TcpConnection::errorHandle, this));
    channel_.setCloseHandler(std::bind(&TcpConnection::closeHandle, this));
}

TcpConnection::~TcpConnection(){
    std::cout << "Disconnected ip :" << inet_ntoa(oppositeaddr_.sin_addr) << std::endl;
    loop_->removeChannelToPoller(&channel_);
    close(fd_);
}


void TcpConnection::readHandle(){
    int rbytes = readData(fd_, bufferin_);

    if(rbytes > 0){
        msgCallback_(shared_from_this(), bufferin_);
    }
    else if(rbytes == 0){
        //closeTcpconnectionCallback_(shared_from_this());
        closeHandle();
    }
    else{
        //错误回调
        std::cout << "错误回调" << std::endl;
    }
}

void TcpConnection::writeHandle(){
    if(closed_) return ;
    int wbytes = writeData(fd_, bufferout_);

    if(wbytes > 0){
        uint32_t events = channel_.getEvents();
        //数据可能没发完，根据buffout的内容判断
        if(bufferout_.size() > 0){
            channel_.setEvents(events | EPOLLOUT);
            loop_->updateChannelToPoller(&channel_);
        }
        else{
            events = events & (~EPOLLOUT);
            loop_->updateChannelToPoller(&channel_);
        }
    }
    else if(wbytes < 0){
        errorHandle();
    }
    else {
        closeHandle();
    }
}

void TcpConnection::closeHandle(){
    if(closed_) return ;
    closeTcpconnectionCallback_(shared_from_this());
    loop_->addTask(std::bind(clearConnection_, shared_from_this()));
    closed_ = true;
}

void TcpConnection::errorHandle(){
    if(closed_) return ;
    errorCallback_(shared_from_this());
    loop_->addTask(std::bind(clearConnection_, shared_from_this()));
    closed_ = true;
}

void TcpConnection::addChannelToEventLoop(){
    loop_->addChannelToPoller(&channel_);
}

void TcpConnection::sendData(const std::string& data){
    bufferout_ += data;
    //在loop中发送
    loop_->addTask(std::bind(&TcpConnection::writeHandle, this));
}

void TcpConnection::shutdown(){
    if(closed_) return ;
    //
    loop_->addTask(std::bind(&TcpConnection::closeHandle, shared_from_this()));
}


//调用read去读fd收到的数据,返回读取到的总字节数
int readData(int fd, std::string& bufferin_){
    int sum = 0, n = 0;
    char buff[BUFFSIZE];
    //边缘触发+循环读
    while(true){
        n = read(fd, buff, BUFFSIZE);

        if(n > 0){
            bufferin_.append(buff, n);
            sum += n;
            if(n < BUFFSIZE) return sum;
            else continue;
        }
        else if(n < 0){
            //EAGAIN 当使用不可阻断I/O 时(O_NONBLOCK), 若无数据可读取则返回此值。
            if(errno == EAGAIN){
                return sum;
            }
            //EINTR 此调用被信号所中断。
            else if(errno == EINTR){
                continue;
            }
            else { //RST?
                return -1;
            }
        }
        else{ //0代表对端关闭连接？FIN
            return 0;
        }
    }
}

//调用write系统调用写fd发送数据,返回写出的数据字节数
int writeData(int fd, std::string& buffout_){
    int sum = 0, n = 0;
    int len = buffout_.size();
    //防止fd缓冲区满，分次写入，每次写BUFFSIZE个
    if(len > BUFFSIZE) len = BUFFSIZE;
    while(true){
        n = write(fd, buffout_.c_str(), len);
        if(n > 0){
            sum += n;
            buffout_.erase(0, len);
            len = buffout_.size();
            if(len > BUFFSIZE) len = BUFFSIZE;
            else if(len == 0) return sum;
        }
        else if(n < 0){
            if(errno == EAGAIN){
                std::cout << "write to fd not finished, maybe fd_buff is full" << std::endl;
                return sum;
            }
            else if(errno == EINTR){
                continue;
            }
            else {
                perror("write error");
                return -1;
            }
        }
        else return 0;
    }
}