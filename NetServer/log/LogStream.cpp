
#include "LogStream.h"
#include <algorithm>

template class FixBuffer<SMALLSIZE>;
template class FixBuffer<LARGESIZE>;

const char digits[] = "9876543210123456789";
const char* zero = digits + 9;

// From muduo
template <typename T>
size_t convert(char buf[], T value) {
    T i = value;
    char* p = buf;

    do {
        int lsd = static_cast<int>(i % 10);
        i /= 10;
        *p++ = zero[lsd];
    } while (i != 0);

    if (value < 0) {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template <typename T>
void LogStream::formatInteger(T v) {
    // buff容不下32个字符的话会被直接丢弃
    if (buff_.available() >= 32) {
        size_t len = convert(buff_.getCur(), v);
        buff_.add(len);
    }
}

LogStream& LogStream::operator<<(short v) {
    *this << static_cast<int>(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned short v) {
    *this << static_cast<unsigned int>(v);
    return *this;
}

LogStream& LogStream::operator<<(int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned int v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(unsigned long long v) {
    formatInteger(v);
    return *this;
}

LogStream& LogStream::operator<<(double v) {
    if (buff_.available() >= 32) {
        int len = snprintf(buff_.getCur(), 32, "%.12g", v);
        buff_.add(len);
    }
    return *this;
}

LogStream& LogStream::operator<<(long double v) {
    if (buff_.available() >= 32) {
        int len = snprintf(buff_.getCur(), 32, "%.12Lg", v);
        buff_.add(len);
    }
    return *this;
}