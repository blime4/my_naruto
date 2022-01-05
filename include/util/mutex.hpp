#ifndef NARUTO_UTIL_MUTEX_H
#define NARUTO_UTIL_MUTEX_H

#include <pthread.h>

namespace naruto
{
    class Mutex
    {
    public:
        Mutex() { pthread_mutex_init(&mutex_, NULL); }
        ~Mutex() { pthread_mutex_destroy(&mutex_); }

        void lock() { pthread_mutex_lock(&mutex_); };
        void unlock() { pthread_mutex_unlock(&mutex_); };

        pthread_mutex_t *getMutex() { return &mutex_; }

    private:
        pthread_mutex_t mutex_;
    };

    class MutexGuard
    {
    public:
        MutexGuard(Mutex &mutex) : mutex_(mutex) { mutex_.lock(); };
        ~MutexGuard() { mutex_.unlock(); };

    private:
        Mutex &mutex_;
    };

}

#endif