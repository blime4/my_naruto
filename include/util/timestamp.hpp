#ifndef NARUTO_UTIL_TIME_STAMP_H_
#define NARUTO_UTIL_TIME_STAMP_H_

#include <stdint.h>
#include <string>
#include <sys/time.h>

namespace naruto
{

    class Timestamp
    {
    public:
        static constexpr uint64_t kMicrosecondsPerSecond = 1000 * 1000;

        Timestamp() : microseconds_from_epoch_(0) {}
        explicit Timestamp(uint64_t microseconds_from_epoch)
            : microseconds_from_epoch_(microseconds_from_epoch) {}

        uint64_t getMicroSecondsFromEpoch() const { return microseconds_from_epoch_; }
        time_t getSec() const { return microseconds_from_epoch_ / kMicrosecondsPerSecond; }
        suseconds_t getUsec() const { return microseconds_from_epoch_ % kMicrosecondsPerSecond; }

        static std::string nowStrTime(){
            time_t timep;
            time(&timep);
            char res[64];
            strftime(res, sizeof(res), "%Y-%m-%d %H:%M:%S", localtime(&timep));
            return res;
        }
        
        static Timestamp now(){
            struct timeval tv;
            if(gettimeofday(&tv, nullptr)){
                return Timestamp();
            }
            return Timestamp(tv.tv_sec * kMicrosecondsPerSecond + tv.tv_usec);
        }

    private:
        uint64_t microseconds_from_epoch_;
    };

    inline bool operator<(const Timestamp &lhs, const Timestamp &rhs)
    {
        return lhs.getMicroSecondsFromEpoch() < rhs.getMicroSecondsFromEpoch();
    }

    inline bool operator==(const Timestamp &lhs, const Timestamp &rhs)
    {
        return lhs.getMicroSecondsFromEpoch() == rhs.getMicroSecondsFromEpoch();
    }

    inline Timestamp operator+(const Timestamp &lhs, uint64_t micro_seconds)
    {
        return Timestamp(lhs.getMicroSecondsFromEpoch() + micro_seconds);
    }

    inline int64_t operator-(const Timestamp &lhs, const Timestamp &rhs)
    {
        return lhs.getMicroSecondsFromEpoch() - rhs.getMicroSecondsFromEpoch();
    }

} // namespace naruto

#endif