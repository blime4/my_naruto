#ifndef NARUTO_LOGGER_H
#define NARUTO_LOGGER_H

#include <map>
#include <string>
#include <list>
#include <functional>
#include <stdarg.h>
#include <string.h>
#include <algorithm>

#include "log_appender_interface.hpp"
#include "log_config.hpp"

#include "mutex.hpp"
#include "singleton.hpp"
#include "timestamp.hpp"

namespace naruto
{
    std::string getLogLevelStr(LogLevel log_level)
    {
        switch (log_level)
        {
#define XX(name)         \
    case LogLevel::name: \
        return #name;    \
        break;

            XX(DEBUG);
            XX(INFO);
            XX(WARN);
            XX(ERROR);
            XX(FATAL);
#undef XX
        default:
            return "UNKNOW";
        }
        return "UNKNOW";
    }
    extern LogConfig kLogConfig;
    class Logger
    {
    public:

        static Logger *getLogger() { return Singleton<Logger>::getInstance(); }
        static void setGlobalLogger(const LogConfig &log_config) { kLogConfig = log_config; }

        template <class F, class... Args>
        void registerHandler(F &&f, Args &&...args)
        {
            using RetType = decltype(f(args...));
            auto task = std::make_shared<RetType()>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            functors_.emplace([task]()
                              { (*task)(); });
        }

        void info(const char *format, ...)
        {
            if (!format)
            {
                return;
            }
            va_list ap;
            va_start(ap, format);
            writeLog(LogLevel::INFO, __FILE__, __FUNCTION__, __LINE__, format, ap);
            va_end(ap);
        };
        void debug(const char *format, ...)
        {
            if (!format)
            {
                return;
            }
            va_list ap;
            va_start(ap, format);
            writeLog(LogLevel::DEBUG, __FILE__, __FUNCTION__, __LINE__, format, ap);
            va_end(ap);
        };
        void warn(const char *format, ...)
        {
            if (!format)
            {
                return;
            }
            va_list ap;
            va_start(ap, format);
            writeLog(LogLevel::WARN, __FILE__, __FUNCTION__, __LINE__, format, ap);
            va_end(ap);
        };
        void error(const char *format, ...)
        {
            if (!format)
            {
                return;
            }
            va_list ap;
            va_start(ap, format);
            writeLog(LogLevel::ERROR, __FILE__, __FUNCTION__, __LINE__, format, ap);
            va_end(ap);
        };
        void fatal(const char *format, ...)
        {
            if (!format)
            {
                return;
            }
            va_list ap;
            va_start(ap, format);
            writeLog(LogLevel::FATAL, __FILE__, __FUNCTION__, __LINE__, format, ap);
            va_end(ap);
        };

        //需要加锁的
        void addAppender(const std::string &appender_name, LogAppenderInterface::Ptr appender)
        {
            MutexGuard guard(mutex_);
            appenders_[appender_name] = appender;
        };
        void delAppender(const std::string &appender_name)
        {
            MutexGuard guard(mutex_);
            for (auto it = appenders_.begin(); it != appenders_.end();)
            {
                if (it->first == appender_name)
                {
                    it = appenders_.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        };

        void clearAppenders() { appenders_.clear(); };

    private:
        void writeLog(LogLevel log_level, const char *file_name,
                      const char *function_name, int32_t line_num,
                      const char *fmt, va_list ap)
        {
            if (log_level < kLogConfig.log_level)
            {
                return;
            }
            std::string str_result;
            if (nullptr != fmt)
            {
                size_t length = vprintf(fmt, ap) + 1; //获取格式化字符串长度
                std::vector<char> fmt_bufs(length,
                                           '\0'); //创建用于存储格式化字符串的字符数组
                int writen_n = vsnprintf(&fmt_bufs[0], fmt_bufs.size(), fmt, ap);
                if (writen_n > 0)
                {
                    str_result = &fmt_bufs[0];
                }
            }
            if (str_result.empty())
            {
                return;
            }
            const auto &getSourceFileName = [](const char *file_name)
            {
                return strrchr(file_name, '/') ? strrchr(file_name, '/') + 1 : file_name;
            };

            std::string prefix;
            prefix.append(Timestamp::nowStrTime() + "-");
            prefix.append(getLogLevelStr(log_level) + "-");
            prefix.append(getSourceFileName(file_name));
            prefix.append("-");
            prefix.append(function_name);
            prefix.append("-");
            prefix.append(std::to_string(line_num) + "-");
            prefix.append(str_result);
            while (!functors_.empty())
            {
                Task task = std::move(functors_.front());
                functors_.pop_front();
                task();
            }
            MutexGuard guard(mutex_);
            for (const auto &appender : appenders_)
            {
                appender.second->append(prefix.data(), prefix.size());
            }
        }

    private:
        using Task = std::function<void()>;
        Mutex mutex_;
        std::map<std::string, LogAppenderInterface::Ptr> appenders_;
        std::list<Task> functors_;
    };

}

#define log_fatal(fmt, args...) naruto::Logger::fatal(fmt.##args)
#define log_error(fmt, args...) naruto::Logger::error(fmt.##args)
#define log_warn(fmt, args...) naruto::Logger::warn(fmt.##args)
#define log_debug(fmt, args...) naruto::Logger::debug(fmt.##args)
#define log_info(fmt, args...) naruto::Logger::info(fmt.##args)

#endif
