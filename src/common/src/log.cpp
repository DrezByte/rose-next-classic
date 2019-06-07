#include "log.h"

using namespace Rose::Common;

Log g_LOG;

Log::Log ():
	_buffer{}
{
}

Log::~Log ()
{
}

void Log::CS_ODS (unsigned short log_mode, char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	log(LogLevel::Debug, "", 0, msg, args);
	va_end(args);

}

void Log::OutputString (unsigned short log_mode, char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	g_LOG.log(LogLevel::Debug, "", 0, msg, args);
	va_end(args);
}

void Log::log(LogLevel level, const char* file, uint32_t line, const char* message, va_list args) {
	std::lock_guard<std::mutex> lock(_log_mutex);

	try {
		vsprintf(_buffer, message, args);
	}
	catch (...) {
		return;
	}

	Rose::Common::logger_write(level, file, line, _buffer);

}

void Log::log(LogLevel level, const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(level, file, line, message, args);
	va_end(args);
}

void Log::trace(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Trace, file, line, message, args);
	va_end(args);
}

void Log::	debug(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Debug, file, line, message, args);
	va_end(args);
}

void Log::info(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Info, file, line, message, args);
	va_end(args);
}
void Log::warn(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Warn, file, line, message, args);
	va_end(args);
}

void Log::error(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Error, file, line, message, args);
	va_end(args);
}