#include <sys/epoll.h>
#include <signal.h>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include "TcpServer.h"
#include "EventLoop.h"
#include "TcpConnection.h"
using namespace std;

typedef shared_ptr<TcpConnection> spConnection;

EventLoop* lp;

void msgcb(const spConnection& tcpcon, string& data){
    cout << data << endl;
}

void cb(spConnection tcpcon){
    cout << "callback" << endl;
}
void ercb(spConnection tcpcon){
    cout << "error callback" << endl;
}
void clcb(spConnection tcpcon){
    cout << "close callback" << endl;
}

void sig_handle(int sig){
    lp->stop();
}

void addsig(int sig){
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = sig_handle;
    sigfillset(&sa.sa_mask);
    sigaction(sig, &sa, NULL);
}

int main(int argc, char const *argv[]){

    signal(SIGINT, sig_handle);

    EventLoop loop;
    lp = &loop;
    TcpServer server(&loop, 8888, 2);
    server.setMessageCallback(std::bind(msgcb, placeholders::_1, placeholders::_2));
    server.setWriteFinishedCallback(std::bind(cb, placeholders::_1));
    server.setCloseTcpconnection(std::bind(clcb, placeholders::_1));
    server.setErrorCallback_(std::bind(ercb, placeholders::_1));
    
    server.start();

    loop.loop();

    return 0;
}

//'Droid Sans Mono', 'monospace', monospace, 'Droid Sans Fallback'