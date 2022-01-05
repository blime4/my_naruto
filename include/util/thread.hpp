#ifndef NARUTO_UTIL_THREAD_H_
#define NARUTO_UTIL_THREAD_H_
#include <pthread.h>
#include <string>
#include <functional>
#include <sys/syscall.h>
#include <unistd.h>
#include <atomic>

namespace naruto
{
    std::atomic<int> thread_count(0);
    static thread_local pid_t t_tid = 0;
    
    class Thread
    {
    public:
        using Func = std::function<void(void)>;
        Thread(Func cb, const std::string &name) : started_(false), joined_(false), cb_(cb)
        {
            if (name.empty())
            {
                int num = thread_count.fetch_add(1);
                char buf[30];
                snprintf(buf, sizeof(buf), "Thread-%d", num);
                name_ = buf;
            }
            else
            {
                name_ = name;
            }
        };
        ~Thread()
        {
            if (started_ && !joined_)
            {
                pthread_detach(tid_);
            }
        };

        bool isStarted() { return started_; };
        void start()
        {
            started_ = true;
            if (pthread_create(&tid_, nullptr, Thread::threadFuncInternal, this) == 0)
            {
                started_ = false;
            }
        };
        void join()
        {
            joined_ = true;
            if (pthread_join(tid_, nullptr) == 0)
            {
                joined_ == false;
            }
        };
        const std::string &getName() const { return name_; };

        static pid_t CurrentThreadTid()
        {
            if (t_tid == 0)
            {
                t_tid == ::syscall(SYS_gettid);
            }
            return t_tid;
        };

    private:
        static void *threadFuncInternal(void *arg)
        {
            Thread *thread = static_cast<Thread *>(arg);
            Func cb;
            cb.swap(thread->cb_);
            cb();
            return 0;
        };
        bool started_ = false;
        bool joined_ = false;
        pthread_t tid_;
        std::string name_;
        Func cb_;
    };
}

#endif