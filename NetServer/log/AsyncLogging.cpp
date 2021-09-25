#include "AsyncLogging.h"
#include "File.h"

AsyncLogging::AsyncLogging(const std::string& logfilename, int flushInterval):
    currBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    fullBuffers_(),
    mutex_(),
    cv_(),
    flushInterval_(flushInterval),
    running_(false),
    thread_(),
    logfilename_(logfilename)
{
    currBuffer_->bzero();
    nextBuffer_->bzero();
    fullBuffers_.reserve(10);
}

AsyncLogging::~AsyncLogging(){
    std::cout << "~AsyncLogging" << std::endl;
    if(running_) stop();
}

void AsyncLogging::start(){
    running_ = true;
    thread_ = std::thread(&AsyncLogging::threadFunc, this);
}

void AsyncLogging::stop(){
    running_ = false;
    cv_.notify_all();
    thread_.join();
}

void AsyncLogging::append(const char* logline, int len){
    std::unique_lock<std::mutex> lock(mutex_);
    if(currBuffer_->available() >= len){
        currBuffer_->append(logline, len);
    }
    else{
        fullBuffers_.emplace_back(currBuffer_.release());
        if(nextBuffer_ != nullptr){
            currBuffer_ = std::move(nextBuffer_);
        }
        else{
            currBuffer_.reset(new Buffer);
        }
        currBuffer_->append(logline, len);
        cv_.notify_all();
    }
}

void AsyncLogging::threadFunc(){
    File file(logfilename_);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffToWrite;
    buffToWrite.reserve(10);
    while(running_){
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if(fullBuffers_.empty()){
                cv_.wait_for(lock, std::chrono::seconds(flushInterval_));
            }
            fullBuffers_.emplace_back(currBuffer_.release());
            currBuffer_ = std::move(newBuffer1);
            buffToWrite.swap(fullBuffers_);
            if(nextBuffer_ == nullptr){
                nextBuffer_ = std::move(newBuffer2);
            }
        }
        if(buffToWrite.size() > 20){
            //可以在这里写入一下日志太多错误
            buffToWrite.erase(buffToWrite.begin() + 2, buffToWrite.end());
        }

        for(BufferPtr& bp : buffToWrite){
            file.append(bp->getData(), bp->length());
        }

        if(buffToWrite.size() > 2) buffToWrite.resize(2);

        if(newBuffer1 == nullptr){
            newBuffer1.swap(buffToWrite.back());
            buffToWrite.pop_back();
            newBuffer1->reset();
        }
        if(newBuffer2 == nullptr){
            newBuffer2.swap(buffToWrite.back());
            buffToWrite.pop_back();
            newBuffer2->reset();
        }
        buffToWrite.clear();
        file.flush();
    }
    file.flush();
}