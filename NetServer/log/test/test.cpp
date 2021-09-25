#include "../Logger.h"
#include "../File.h"

int main(){
    {
        LOG_INFO << "This is log";
    }
    LOG_INFO << "This is log, too";
    int a = 1000000000;
    while(a--) {
        // LOG_INFO << "This is log, too";
        // LOG_INFO << a;
    }
    return 0;
}