
#ifndef _FILE_H_
#define _FILE_H_

#include <string>
#include <cstdio>

class File
{
private:
    /* data */
    FILE* fp_;

    int write(const char* logline, int len);

    char buff_[64 * 1024];

public:
    File(const std::string& filename);
    ~File();

    void append(const char* logline, int len);
    void flush();
};

#endif