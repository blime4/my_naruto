#ifndef NARUTO_LOG_FILE_H_
#define NARUTO_LOG_FILE_H_

#include <memory>
#include <string>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>

#include "file_writer_type.hpp"

namespace naruto
{


    std::string getHostName()
    {
        char buf[256];
        if (::gethostname(buf, sizeof(buf)) == 0)
        {
            buf[sizeof(buf) - 1] = '\0';
            return buf;
        }
        else
        {
            return "unknownhost";
        }
    }

    std::string getLogFileName(const std::string &basename, time_t *now)
    {
        std::string filename;
        filename.reserve(basename.size() + 64);
        filename = basename;

        char timebuf[32];
        struct tm tm;
        *now = time(NULL);
        gmtime_r(now, &tm);
        strftime(timebuf, sizeof(timebuf), ".%Y%m%d-%H%M%S.", &tm);
        filename += timebuf;
        filename += getHostName();
        char pidbuf[32];
        snprintf(pidbuf, sizeof(pidbuf), ".%d", getpid());
        filename += pidbuf;
        filename += ".log";

        return filename;
    }


    class FileWriter
    {
    public:
        FileWriter() = default;
        virtual ~FileWriter() = default;
        virtual void append(const char *msg, int32_t len) = 0;
        virtual void flush() = 0;
        virtual uint32_t writtenBytes() const = 0;
    };

        class MMAPFileWriter : public FileWriter
    {
    public:
        MMAPFileWriter(const std::string &basename, uint32_t mem_size)
            : mem_size_(mem_size), writed_(0)
        {
            if (fd_ >= 0)
            {
                close(fd_);
            }
            fd_ = open(basename.c_str(), O_RDWR | O_CREAT | S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd_ < 0)
            {
                fprintf(stderr, "open new file failed: errno=%d", errno);
            }
            else
            {
                int n = ftruncate(fd_, mem_size_);
                (void)n;
                buffer_ = (char *)mmap(NULL, mem_size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
                if (buffer_ == MAP_FAILED)
                {
                    fprintf(stderr, "mmap file failed, errno=%d", errno);
                }
            }
        }
        ~MMAPFileWriter()
        {
            if (fd_ >= 0)
            {
                close(fd_);
                fd_ = -1;
            }
            if (buffer_ != MAP_FAILED)
            {
                munmap(buffer_, mem_size_);
            }
        }

        void append(const char *msg, int32_t len)
        {
            if (len > mem_size_ - writed_)
            {
                fprintf(stderr, "mmap memory overflow , errno=%d", errno);
                return;
            }
            memcpy(buffer_ + writed_, msg, len);
            writed_ += len;
        }
        void flush()
        {
            if (buffer_ != MAP_FAILED)
            {
                msync(buffer_, mem_size_, MS_ASYNC);
            }
        };
        uint32_t writtenBytes() const { return writed_; }

    private:
        int fd_;
        char *buffer_;
        int32_t mem_size_;
        int32_t writed_;
    };

    class AppendFileWriter : public FileWriter
    {
    public:
        AppendFileWriter(const std::string &filename)
            : fp_(::fopen(filename.c_str(), "ae"))
        {
            ::setbuffer(fp_, buffer_, sizeof(buffer_));
        }
        ~AppendFileWriter()
        {
            if (fp_)
            {
                ::fclose(fp_);
            }
        }
        void append(const char *msg, int32_t len)
        {
            size_t n = ::fwrite_unlocked(msg, 1, len, fp_);
            size_t remain = len - n;
            while (remain > 0)
            {
                size_t x = ::fwrite_unlocked(msg + n, 1, remain, fp_);
                if (x == 0)
                {
                    int err = ferror(fp_);
                    if (err)
                    {
                        fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
                    }
                    break;
                }
                n += x;
                remain = len - n; // remain -= x
            }

            writed_ += len;
        }
        void flush()
        {
            ::fflush(fp_);
        }
        uint32_t writtenBytes() const { return writed_; }

    private:
        FILE *fp_;
        char buffer_[64 * 1024];
        uint32_t writed_ = 0;
    };

    class LogFile
    {
    public:
        LogFile(const std::string &basename,
                int32_t roll_size, int32_t flush_interval,
                int32_t check_interval, FileWriterType file_writer_type)
            : basename_(basename), roll_size_(roll_size),
              flush_interval_(flush_interval), check_freq_count_(check_interval),
              count_(0), start_of_period_(0), last_roll_(0), last_flush_(0)
        {
            time_t now = 0;
            std::string filename = getLogFileName(basename_, &now);   

		// FIXME: 依赖于具体而不是依赖于抽象
            if (file_writer_type == FileWriterType::MMAPFILE)
            {
                file_ = std::make_shared<MMAPFileWriter>(filename, roll_size_);
            }
            else
            {
                file_ = std::make_shared<AppendFileWriter>(filename);
            }
            file_writer_type_ = file_writer_type;
            rollFile();
        }
        ~LogFile() = default;
        void append(const char *msg, int32_t len)
        {
            file_->append(msg, len);

		
            if (file_->writtenBytes() > roll_size_)
            {
                count_ = 0;
                time_t now = ::time(NULL);
                time_t this_period = now / kRollPerSeconds * kRollPerSeconds;
                if (this_period != start_of_period_)
                {
                    rollFile();
                }
		// TODO : 跨天就对日志进行切割,  为什么总是切割，丢失日志呢？
                else if (now - last_flush_ > flush_interval_)
                {
                    last_flush_ = now;
                    file_->flush();
                }
            }
        }
        void flush() { file_->flush(); }
        bool rollFile()
        {
            time_t now = 0;
            std::string filename = getLogFileName(basename_, &now);
            time_t start = now / kRollPerSeconds * kRollPerSeconds;

            if (now > last_roll_)
            {
                last_roll_ = now;
                last_flush_ = now;
                start_of_period_ = start;

		// FIXME: 依赖于具体而不是抽象
                if (file_writer_type_ == FileWriterType::MMAPFILE)
                {
                    file_.reset(new MMAPFileWriter(filename, roll_size_));
                }
                else
                {
                    file_.reset(new AppendFileWriter(filename));
                }
            }

            return true;
        };

    private:
        std::string basename_;
        uint32_t roll_size_;
        uint32_t flush_interval_;
        uint32_t check_freq_count_;
        uint32_t count_;
        time_t start_of_period_;
        time_t last_roll_;
        time_t last_flush_;
        std::shared_ptr<FileWriter> file_;
        FileWriterType file_writer_type_;
        constexpr static int kRollPerSeconds = 60 * 60 * 24;
    };

}

#endif //
