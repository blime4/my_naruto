#ifndef NARUTO_LOG_CONFIG_H
#define NARUTO_LOG_CONFIG_H

#include <stdint.h>
#include <string>

#include "file_writer_type.hpp"
#include "log_level.hpp"

namespace naruto {
struct LogConfig
{
    uint32_t log_buffer_size = 4000;
    uint32_t log_buffer_nums = 2;
    LogLevel log_level = LogLevel::INFO;

    struct FileOption{
        
        std::string file_path;
        uint32_t log_flush_file_size = 10;
        uint32_t log_flush_interval = 1;
        FileWriterType file_writer_type = FileWriterType::APPENDFILE;
    } file_option;
};

LogConfig kLogConfig;



}
#endif
