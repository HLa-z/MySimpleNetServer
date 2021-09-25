
#ifndef _LOGSTREAM_H_
#define _LOGSTREAM_H_

#include <string>
#include <cstring>
#include <iostream>
const int SMALLSIZE = 4096;
const int LARGESIZE = 4096 * 1024;

template<int BUFFSIZE>
class FixBuffer
{
private:
    char data_[BUFFSIZE];
    char* cur_;

public:
    FixBuffer() : cur_(data_){}
    ~FixBuffer(){}

    int length() const { return static_cast<int>(cur_ - data_); }
    const char* getData() const { return data_; }
    char* getCur() const {return cur_; }
    int available() const { return static_cast<int>(data_ + BUFFSIZE - cur_ - 2); }

    void add(int len) { cur_ += len; }
    void reset() { cur_ = data_; }
    void bzero() { memset(data_, 0, sizeof(data_)); }

    void append(const char* logline, int len){
        if(available() >= len){
            memcpy(cur_, logline, len);
            cur_ += len;
        }
    }
};

class LogStream
{
public:
    typedef FixBuffer<SMALLSIZE> Buffer;
    
private:
    /* data */
    Buffer buff_;

    template<typename T>
    void formatInteger(T v);

public:
    LogStream() : buff_(){}
    ~LogStream(){}

    LogStream& operator << (bool v) {
        buff_.append(v ? "1" : "0", 1);
        return *this;
    }

    LogStream& operator << (short);
    LogStream& operator << (unsigned short);
    LogStream& operator << (int);
    LogStream& operator << (unsigned int);
    LogStream& operator << (long);
    LogStream& operator << (unsigned long);
    LogStream& operator << (long long);
    LogStream& operator << (unsigned long long);

    LogStream& operator << (const void*);

    LogStream& operator << (float v) {
        *this << static_cast<double>(v);
        return *this;
    }
    LogStream& operator << (double);
    LogStream& operator << (long double);

    LogStream& operator << (char v) {
        buff_.append(&v, 1);
        return *this;
    }

    LogStream& operator << (const char* str) {
        if (str)
            buff_.append(str, strlen(str));
        else
            buff_.append("(null)", 6);
        return *this;
    }

    LogStream& operator << (const unsigned char* str) {
        return operator << (reinterpret_cast<const char*>(str));
    }

    LogStream& operator << (const std::string& v) {
        buff_.append(v.c_str(), v.size());
        return *this;
    }

    const Buffer& getBuffer() const { return buff_; }
};



#endif