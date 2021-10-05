#include "Socket.h"
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

Socket::Socket(){
    serverfd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd_ == -1){
        perror("socket()");
        exit(-1);
    }
    //std::cout << "create Socket : fd = " << serverfd_ << std:: endl;
}

Socket::~Socket(){
    close(serverfd_);
    std::cout << "close socket" << std::endl;
}

//int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);
void Socket::setAddrReuse(){
    int on = 1;
    if(setsockopt(serverfd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0){
        perror("setsockopt error");
    }
}

void Socket::setNoBlock(){
    int flag = fcntl(serverfd_, F_GETFL);
    if(flag < 0){
        perror("fcntl(serverfd_, F_GETFL)");
        exit(-1);
    }
    if(fcntl(serverfd_, F_SETFL, flag | O_NONBLOCK) < 0){
        perror("fcntl(serverfd_, flag | O_NONBLOCK)");
        exit(-1);
    }
    std::cout << "set server no block..." << std::endl;
}

bool Socket::Bind(int port){
    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(serverfd_, (struct sockaddr*)&saddr, sizeof(saddr)) == -1){
        perror("Bind error");
        close(serverfd_);
        exit(-1);
    }
    std::cout << "Binding..." << std::endl;
    return true;
}

bool Socket::Listen(){
    if(listen(serverfd_, MAX_LISTEN_NUM) < 0){
        perror("Listen error!");
        Close();
        exit(-1);
    }
    std::cout << "Listening..." << std::endl;
    return true;
}

int Socket::Accept(struct sockaddr_in& client_addr){
    socklen_t len = sizeof(client_addr);
    int client_fd = accept(serverfd_, (struct sockaddr*)&client_addr, &len);
    //std::cout << "server accept client_fd : " << client_fd << std::endl;
    return client_fd;
}

bool Socket::Close(){
    close(serverfd_);
    std::cout << "server close..." << std::endl;
    return true;
}

//test
// int main(int argc, char const *argv[]){
//     Socket server;
//     server.Bind(8080);
//     server.Listen();
//     sockaddr_in caddr;
//     memset(&caddr, 0, sizeof(caddr));
//     int cfd = server.Accept(caddr);
//     return 0;
// }
