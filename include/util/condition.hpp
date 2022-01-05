#ifndef NARUTO_UTIL_CONDITION_H_
#define NARUTO_UTIL_CONDITION_H_

#include <pthread.h>
#include <stdint.h>
#include <errno.h>

#include "mutex.hpp"


namespace naruto
{
    class Condition
    {
    public:
        explicit Condition(Mutex &mutex) : mutex_(mutex) { pthread_cond_init(&condvar_, nullptr); };
        ~Condition() { pthread_cond_destroy(&condvar_); };

        void wait(){pthread_cond_wait(&condvar_, mutex_.getMutex());};

        bool waitForSeconds(int32_t seconds){
            struct timespec abstime;
            clock_gettime(CLOCK_REALTIME, &abstime);
            const int64_t kNanoSecondsPerSecond = 1000000000;
            int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);
            abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
            abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);
            return ETIMEDOUT == pthread_cond_timedwait(&condvar_, mutex_.getMutex(), &abstime);
        };

        void notifyOne(){pthread_cond_signal(&condvar_);};
        void notifyAll(){pthread_cond_broadcast(&condvar_);};

    private:
        Mutex &mutex_;
        pthread_cond_t condvar_;
    };

} // namespace naruto
#endif