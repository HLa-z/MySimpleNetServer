#include "../EventLoop.h"
#include "HttpServer.h"
#include <signal.h>

EventLoop* mainloop = nullptr;

void sigint_handle(int sig){
    mainloop->stop();
}

int main(int argc, char const *argv[]){
    if(argc != 4) exit(0);
    int port = atoi(argv[1]);
    int io_nums = atoi(argv[2]);
    int wk_nums = atoi(argv[3]);
    
    signal(SIGINT, sigint_handle);

    EventLoop loop;
    mainloop = &loop;
    HttpServer httpServer(mainloop, port, io_nums, wk_nums);
    
    httpServer.Start();
    loop.loop();
    return 0;
}
