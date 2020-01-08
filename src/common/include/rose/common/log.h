#ifndef CLOG_H
#define CLOG_H
#pragma once

#include "rose/common/common_interface.h"
#include <mutex>

#define LOG_TRACE(msg, ...) g_LOG.trace(__FILE__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_DEBUG(msg, ...) g_LOG.debug(__FILE__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...) g_LOG.info(__FILE__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_WARN(msg, ...) g_LOG.warn(__FILE__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...) g_LOG.error(__FILE__, __LINE__, msg, ##__VA_ARGS__);

// -- Legacy defines for OutputString/CS_ODS
#define LOG_NORMAL 0x01
#define LOG_DEBUG_ 0x04
// -- End legacy defines

#define LOG_BUFFER_SIZE 1024

class Log {
private:
    char _buffer[LOG_BUFFER_SIZE];
    std::mutex _log_mutex;

public:
    Log();
    ~Log();

    // -- Legacy logging functions (TODO: deprecated, references need to be removed)
    void static OutputString(unsigned short wLogMODE, char* fmt, ...);
    void CS_ODS(unsigned short wLogMODE, char* fmt, ...);
    void CS_ODS(unsigned short log_mode, const char* msg, ...);

    // -- End Legacy

    // New logging facades
    void log(Rose::Common::LogLevel level,
        const char* file,
        uint32_t line,
        const char* message,
        va_list args);

    void
    log(Rose::Common::LogLevel level, const char* file, uint32_t line, const char* message, ...);

    void trace(const char* file, uint32_t line, const char* message, ...);
    void debug(const char* file, uint32_t line, const char* message, ...);
    void info(const char* file, uint32_t line, const char* message, ...);
    void warn(const char* file, uint32_t line, const char* message, ...);
    void error(const char* file, uint32_t line, const char* message, ...);
};

#define LogString g_LOG.OutputString

extern Log g_LOG;

#endif // CLOG_H
