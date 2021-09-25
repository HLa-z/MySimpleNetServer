
#include "File.h"

File::File(const std::string& filename): 
    fp_(fopen(filename.c_str(), "a+"))
{
    setbuffer(fp_, buff_, sizeof(buff_));
}

File::~File(){
    fclose(fp_);
}

int File::write(const char* logline, int len){
    return fwrite_unlocked(logline, 1, len, fp_);
}

void File::append(const char* logline, int len){
    int remain = len, n = 0;
    while(remain > 0){
        int x = this->write(logline + n, remain);
        if (x == 0) {
            int err = ferror(fp_);
            if (err) fprintf(stderr, "File::append() failed !\n");
            break;
        }
        n += x;
        remain = len - n;
    }
}

void File::flush(){
    fflush(fp_);
}