#ifndef NARUTO_UTIL_COUNT_DOWN_LATH_H_
#define NARUTO_UTIL_COUNT_DOWN_LATH_H_

#include "condition.hpp"
#include "mutex.hpp"

namespace naruto
{
    class CountDownLatch
    {
    public:
        explicit CountDownLatch(int count):count_(count), mutex_(), condition_(mutex_){};
        void wait(){
            MutexGuard guard(mutex_);
            while(count_ > 0){
                condition_.wait();
            }
        };
        void countDown(){
            MutexGuard guard(mutex_);
            count_ --;
            if(count_ == 0){
                condition_.notifyOne();
            }
        };
        inline int32_t getCount() const
        {
            MutexGuard lk(mutex_);
            return count_;
        }

    private:
        int32_t count_ = 0;
        mutable Mutex mutex_;
        Condition condition_;
    };
} // namespace naruto

#endif