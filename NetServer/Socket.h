/*
 * socket的封装 
 */

#ifndef _SOCKET_H_
#define _SOCKET_H_

#define MAX_LISTEN_NUM 2048

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Socket
{
private:
    /* data */
    //服务器监听描述符
    int serverfd_;
    
public:
    Socket(/* args */);
    ~Socket();

    //获取fd
    int getfd() const { return serverfd_; }

    //设置地址重用
    void setAddrReuse();

    //设置socket非阻塞
    void setNoBlock();

    //绑定地址和端口
    bool Bind(int port);

    //监听
    bool Listen();

    //接收
    int Accept(struct sockaddr_in& client_addr);

    //关闭
    bool Close();
};


#endif