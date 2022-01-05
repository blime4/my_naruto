#ifndef NARUTO_UTIL_SINGLETON_H
#define NARUTO_UTIL_SINGLETON_H
#include <pthread.h>

namespace naruto {

template<typename T> class Singleton{

public:
    static T* getInstance(){
        pthread_once(&once_control_, &Singleton::init);
        return value_;
    }
    static void destroy(){
        if(value_ != nullptr){
            delete value_;
        }
    }
private:
    Singleton();
    ~Singleton();
    static void init(){
        value_ = new T();
        ::atexit(destroy);
    }
    static T* value_;
    static pthread_once_t once_control_;
};

template<typename T>
pthread_once_t Singleton<T>::once_control_ = PTHREAD_ONCE_INIT;

template<typename T>
T *Singleton<T>::value_ = nullptr;
}

#endif