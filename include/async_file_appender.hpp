#ifndef NARUTO_ASYNC_FILE_APPENDER_H_
#define NARUTO_ASYNC_FILE_APPENDER_H_
#include <vector>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>


#include "mutex.hpp"
#include "thread.hpp"
#include "condition.hpp"
#include "count_down_latch.hpp"

#include "log_appender_interface.hpp"
#include "log_buffer.hpp"
#include "log_file.hpp"
#include "log_config.hpp"

namespace naruto
{

    extern LogConfig kLogConfig;
    class AsyncFileAppender : public LogAppenderInterface
    {
    public:
        AsyncFileAppender(const std::string &basename)
            : started_(false), running_(false),
              persist_period_(kLogConfig.file_option.log_flush_interval),
              basename_(basename), cond_(mutex_), countdown_latch_(1),
              persist_thread_(std::bind(&AsyncFileAppender::threadFunc, this), "AsyncLogging"),
              cur_buffer_(new LogBuffer(kLogConfig.log_buffer_size))
        {
            mkdir(basename_.c_str(), 0755);
            start();
        }

        ~AsyncFileAppender()
        {
            if (started_)
            {
                stop();
            }
        }
        void append(const char *msg, size_t len)
        {
            MutexGuard guard(mutex_);
            if (cur_buffer_->available() > len)
            {
                cur_buffer_->append(msg, len);
            }
            else
            {
                buffers_.push_back(std::move(cur_buffer_));
                cur_buffer_.reset(new LogBuffer(kLogConfig.log_buffer_size));
                cur_buffer_->append(msg, len);
                cond_.notifyOne();
            }
        }
        void start()
        {
            started_ = true;
            running_ = true;
            persist_thread_.start();
            countdown_latch_.wait();
        }
        void stop()
        {
            started_ = false;
            cond_.notifyOne();
            persist_thread_.join();
        }

    private:
        void threadFunc()
        {
            /*  初始化多线程组件
            1.1 创建一个当前 buffer的指针
            1.2 创建一个持久化的 buffer vector
            1.3 创建一个 logfile对象
            */
            std::unique_ptr<LogBuffer> buffer(new LogBuffer(kLogConfig.log_buffer_size));
            std::vector<std::unique_ptr<LogBuffer>> persist_buffers;
            persist_buffers.reserve(kLogConfig.log_buffer_nums);
            LogFile log_file(basename_, kLogConfig.file_option.log_flush_file_size,
                             kLogConfig.file_option.log_flush_interval, 1024,
                             kLogConfig.file_option.file_writer_type);
            countdown_latch_.countDown();

            while (running_)
            {
                // wake up every persist_per_seconds_ or on Buffer is full
                // 2.1 每秒中被唤醒一次检查当前 buffer是否为空，为空就继续休眠
                // 2.2 不为空的话移动当前 buffer加入 buffers vector中
                MutexGuard gurd(mutex_);
                if (buffers_.empty())
                {
                    cond_.waitForSeconds(persist_period_);
                }
                if (buffers_.empty() && cur_buffer_->length() == 0)
                {
                    continue;
                }
                buffers_.push_back(std::move(cur_buffer_)); // TODO: emplace_back

                // reset  cur_buffer_ and buffers_
                // 2.3 将 buffers交换给创建的持久化 buffer vector用于消费
                persist_buffers.swap(buffers_);
                cur_buffer_ = std::move(buffer);
                // 2.4 重置当前的 buffer
                cur_buffer_->clear();
            }
            // if log is too large, drop it                 // TODO: dont konw why can just drop it 
            if(persist_buffers.size() > kLogConfig.log_buffer_size){
                std::cerr<< "log is too large , drop some" << std::endl;
                persist_buffers.erase(persist_buffers.begin() + 1, persist_buffers.end());
            }
            // persist log
            for (const auto &buffer : persist_buffers) {
                log_file.append(buffer->data(), buffer->length());
            }
	
            // reset buffer and persist_buffers
            buffer = std::move(persist_buffers[0]);
            buffer->clear();
            persist_buffers.clear();

            log_file.flush();
        }
        bool started_;
        bool running_;
        time_t persist_period_;
        std::string basename_;
        Mutex mutex_;
        Condition cond_;
        CountDownLatch countdown_latch_;
        Thread persist_thread_;
        std::unique_ptr<LogBuffer> cur_buffer_;
        std::vector<std::unique_ptr<LogBuffer>> buffers_;
    };
}

#endif