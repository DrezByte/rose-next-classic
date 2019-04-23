#include <windows.h>
#include <stdio.h>
#include "classLOG.h"

using namespace Rose::Common;

extern void WriteLOG  (char *szString);

CLOG g_LOG;

char CLOG::m_StrBUFF[ LOG_STR_SIZE ];

#ifdef	_DEBUG
WORD CLOG::m_wLogMODE[LOG_MAXOUT] = {  0x0ffff, 0x0ffff, 0x0fff };
#else
WORD CLOG::m_wLogMODE[LOG_MAXOUT] = {  LOG_NORMAL, LOG_NORMAL, LOG_NORMAL };
#endif

//-------------------------------------------------------------------------------------------------
CLOG::CLOG ()
{
    ::InitializeCriticalSection( &m_csLOCK );
}
CLOG::~CLOG ()
{
	::DeleteCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
void CLOG::CS_OUT (WORD wLogMODE, char *szMSG)
{
    if ( !(wLogMODE & m_wLogMODE[ LOG_DBGOUT ] ) )
        return;

    ::EnterCriticalSection( &m_csLOCK );

    ::OutputDebugString( szMSG );

    ::LeaveCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
void CLOG::CS_ODS (WORD wLogMODE, char *fmt, ...)
{
    if ( !(wLogMODE & m_wLogMODE[ LOG_DBGOUT ] ) )
        return;

    ::EnterCriticalSection( &m_csLOCK );

    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(m_StrBUFF,fmt,argptr);
        va_end(argptr);
    }
    catch( ... ) {
	    ::LeaveCriticalSection( &m_csLOCK );
        return;
    }

    ::OutputDebugString( m_StrBUFF );

	if ( wLogMODE & m_wLogMODE[ LOG_SCR    ] )
		WriteLOG ( m_StrBUFF );

    ::LeaveCriticalSection( &m_csLOCK );
}

//-------------------------------------------------------------------------------------------------
WORD CLOG::GetLogMODE (t_LOG logType)
{
    if ( logType >= 0 || logType < LOG_MAXOUT )
        return m_wLogMODE[ logType ];
    return 0;
}

//-------------------------------------------------------------------------------------------------
void CLOG::SetLogMODE (WORD wLogMODE, t_LOG logType)
{
    if ( logType >= 0 && logType < LOG_MAXOUT ) {
        WORD wNewMODE;

        if ( wLogMODE & LOG_DEBUG )
            wNewMODE = LOG_NORMAL | LOG_VERBOSE | LOG_DEBUG;
        else
        if ( wLogMODE & LOG_VERBOSE )
            wNewMODE = LOG_NORMAL | LOG_VERBOSE;
        else
        if ( wLogMODE & LOG_NORMAL )
            wNewMODE = LOG_NORMAL;
        else
            wNewMODE = 0;

        m_wLogMODE[ logType ] = wNewMODE;
    }
}

//-------------------------------------------------------------------------------------------------
void CLOG::OutputString (WORD wLogMODE, char *fmt, ...)
{
	if ( !(m_wLogMODE[LOG_SCR] & wLogMODE) && !(m_wLogMODE[LOG_FILE] & wLogMODE) )
		return;

    try {
        va_list argptr;
        va_start(argptr, fmt);
        vsprintf(m_StrBUFF,fmt,argptr);
        va_end(argptr);
    }
    catch( ... ) {
        return;
    }

	::OutputDebugString( m_StrBUFF );
}

void CLOG::log(LogLevel level, const char* message, va_list args) {
	try {
		vsprintf(m_StrBUFF, message, args);
	}
	catch (...) {
		return;
	}

	::EnterCriticalSection(&m_csLOCK);
	Rose::Common::logger_write(level, m_StrBUFF);
	::LeaveCriticalSection(&m_csLOCK);

}

void CLOG::log(LogLevel level, const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(level, message, args);
	va_end(args);
}

void CLOG::trace(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Trace, message, args);
	va_end(args);
}

void CLOG::	debug(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Debug, message, args);
	va_end(args);
}

void CLOG::info(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Info, message, args);
	va_end(args);
}
void CLOG::warn(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Warn, message, args);
	va_end(args);
}

void CLOG::error(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(LogLevel::Error, message, args);
	va_end(args);
}