
#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "Arduino.h"

/**
 * Debug constants
 */
#include "common.h"

/**
 * Sensor diagnostic macros
 */
#ifdef DIAGNOSTICS

#define DIAG( args... ) \
	Serial.print( args ); \
	Serial.print( "\t" );

#define DIAGLN( args... ) \
	Serial.println( args );

#else

#define DIAG( args... )
#define DIAGLN( args... )

#endif // DIAGNOSTICS


/**
 * Error tracing macros
 */
#if ERRORLEVEL > 0
void __logError( uint8_t level, const char *fmt, ... );
void __logErrorF( uint8_t level, const __FlashStringHelper* msg );
void __endError();

#define __LOG_F( lvl, msg ) { __logErrorF( lvl, F(msg) ); __endError(); }
#define __LOG( lvl, args... ) { __logError( lvl, args ); __endError(); }
#define __LOG_OPEN( lvl, args... ) { __logError( lvl, args ); }
#define __LOG_END() { __endError(); }

#define FATAL_F( msg ) __LOG_F( 0, msg );
#define FATAL( args... ) __LOG( 0, args );
#define FATAL_OPEN( args... ) __LOG_OPEN( 0, args );
#define FATAL_END() __LOG_END();
#else
#define FATAL_F( msg )
#define FATAL( args... )
#define FATAL_OPEN( args... )
#define FATAL_END()
#endif

#if ERRORLEVEL > 1
#define ERROR_F( msg ) __LOG_F( 1, msg );
#define ERROR( args... ) __LOG( 1, args );
#define ERROR_OPEN( args... ) __LOG_OPEN( 1, args );
#define ERROR_END() __LOG_END();
#else
#define ERROR_F( msg )
#define ERROR( args... )
#define ERROR_OPEN( args... )
#define ERROR_END()
#endif

#if ERRORLEVEL > 2
#define WARN_F( msg ) __LOG_F( 2, msg );
#define WARN( args... ) __LOG( 2, args );
#define WARN_OPEN( args... ) __LOG_OPEN( 2, args );
#define WARN_END() __LOG_END();
#else
#define WARN_F( msg )
#define WARN( args... )
#define WARN_OPEN( args... )
#define WARN_END()
#endif

#if ERRORLEVEL > 3
#define INFO_F( msg ) __LOG_F( 3, msg );
#define INFO( args... ) __LOG( 3, args );
#define INFO_OPEN( args... ) __LOG_OPEN( 3, args );
#define INFO_END() __LOG_END();
#else
#define INFO_F( msg )
#define INFO( args... )
#define INFO_OPEN( args... )
#define INFO_END()
#endif

#if ERRORLEVEL > 4
#define TRACE_F( msg ) __LOG_F( 4, msg );
#define TRACE( args... ) __LOG( 4, args );
#define TRACE_OPEN( args... ) __LOG_OPEN( 4, args );
#define TRACE_END() __LOG_END();
#else
#define TRACE_F( msg )
#define TRACE( args... )
#define TRACE_OPEN( args... )
#define TRACE_END()
#endif

#endif // DEBUG_H
