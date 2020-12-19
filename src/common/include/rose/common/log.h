#ifndef CLOG_H
#define CLOG_H
#pragma once

#include "rose/common/common_interface.h"

#include "fmt/format.h"

#define LOG(level, msg, ...) \
    g_LOG.log(level, __FILE__, __LINE__, msg, __VA_ARGS__)

#define LOG_TRACE(msg, ...) \
    g_LOG.log(Rose::Common::LogLevel::Trace, __FILE__, __LINE__, msg, __VA_ARGS__)

#define LOG_DEBUG(msg, ...) \
    g_LOG.log(Rose::Common::LogLevel::Debug, __FILE__, __LINE__, msg, __VA_ARGS__)

#define LOG_INFO(msg, ...) \
    g_LOG.log(Rose::Common::LogLevel::Info, __FILE__, __LINE__, msg, __VA_ARGS__)

#define LOG_WARN(msg, ...) \
    g_LOG.log(Rose::Common::LogLevel::Warn, __FILE__, __LINE__, msg, __VA_ARGS__)

#define LOG_ERROR(msg, ...) \
    g_LOG.log(Rose::Common::LogLevel::Error, __FILE__, __LINE__, msg, __VA_ARGS__)

// -- Legacy defines for OutputString/CS_ODS
#define LOG_NORMAL 0x01
#define LOG_DEBUG_ 0x04
// -- End legacy defines

#define LOG_BUFFER_SIZE 1024

class Log;
extern Log g_LOG;

class Log {
public:
    // -- Legacy logging functions (TODO: deprecated, references need to be removed)
    template<typename... Args>
    void static OutputString(unsigned short wLogMODE, char* msg, Args&&... args) {
        g_LOG.log(Rose::Common::LogLevel::Debug, "", 0, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void CS_ODS(unsigned short wLogMODE, char* msg, Args&&... args) {
        log(Rose::Common::LogLevel::Debug, "", 0, msg, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void CS_ODS(unsigned short log_mode, const char* msg, Args&&... args) {
        log(Rose::Common::LogLevel::Debug, "", 0, msg, std::forward<Args>(args)...);
    }
    // -- End Legacy

    // New logging facade
    template<typename... Args>
    void log(Rose::Common::LogLevel level,
        const char* file,
        uint32_t line,
        const char* format_str,
        Args&&... args) {

        std::string msg = fmt::format(format_str, args...);
        Rose::Common::logger_write(level, file, line, msg.c_str());
    }
};

#define LogString g_LOG.OutputString

#endif // CLOG_H
