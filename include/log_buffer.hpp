#ifndef NARUTO_LOG_BUFFER_H_
#define NARUTO_LOG_BUFFER_H_
#include <sys/types.h>

namespace naruto {

void cookieStart(){}
void cookieEnd(){}
class LogBuffer{
public:
    LogBuffer(size_t total = 1024*1024*10)
        :total_(total), available_(total), cur_(0){
            data_ = new char[total];
            setCookie(cookieStart);
        }
    ~LogBuffer(){
        delete[] data_;
        setCookie(cookieEnd);
    }

    void clear(){
        cur_ = 0 ;
        available_ = total_ ;
    }
    void append(const char *data, size_t len){
        memcpy(data_ + cur_, data, len);
        cur_ += len;
        available_ -= len;
    }
    const char *data() const {
        return data_;
    }
    size_t length() const{
        return cur_;
    }
    size_t available() const{
        return available_;
    }
    // for GDB
    const char *debugString();
    void setCookie(void (*cookie)()){ cookie_ = cookie; };
private:
    char *data_;
    const size_t total_;
    size_t available_;
    size_t cur_;
    
    void(*cookie_)();
};
}


#endif