#include <windows.h>
#include <stdio.h>
#include "classLOG.h"

using namespace Rose::Common;

CLOG g_LOG;

CLOG::CLOG ():
	_buffer{}
{
}

CLOG::~CLOG ()
{
}

void CLOG::CS_ODS (WORD log_mode, char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	log(LogLevel::Debug, "", 0, msg, args);
	va_end(args);

}

void CLOG::OutputString (WORD log_mode, char *msg, ...)
{
	va_list args;
	va_start(args, msg);
	g_LOG.log(LogLevel::Debug, "", 0, msg, args);
	va_end(args);
}

void CLOG::log(LogLevel level, const char* file, uint32_t line, const char* message, va_list args) {
	std::lock_guard<std::mutex> lock(_log_mutex);

	try {
		vsprintf(_buffer, message, args);
	}
	catch (...) {
		return;
	}

	Rose::Common::logger_write(level, file, line, _buffer);

}

void CLOG::log(LogLevel level, const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(level, file, line, message, args);
	va_end(args);
}

void CLOG::trace(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Trace, file, line, message, args);
	va_end(args);
}

void CLOG::	debug(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Debug, file, line, message, args);
	va_end(args);
}

void CLOG::info(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Info, file, line, message, args);
	va_end(args);
}
void CLOG::warn(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Warn, file, line, message, args);
	va_end(args);
}

void CLOG::error(const char* file, uint32_t line, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Error, file, line, message, args);
	va_end(args);
}