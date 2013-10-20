
#ifndef _DEBUG_H_
#define _DEBUG_H_

/**
 * Debug constants
 */

//! Trace information
/**
 * 0 - No error reporting
 * 1 - Fatal errors only
 * 2 - Include Error-level tracing
 * 3 - Include Warning-level tracing
 * 4 - Include Trace-level tracing
 */
#define ERRORLEVEL 4

//! Sensor diagnostic output
#define DIAGNOSTICS 0

//! Print helpful console messages to Serial port
#define SERIAL_OUT 0


/**
 * Sensor diagnostic macros
 */
#if DIAGNOSTICS

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

#define FATAL( args... ) __logError( 0, args );
#else
#define FATAL( args... )
#endif

#if ERRORLEVEL > 1
#define ERROR( args... ) __logError( 1, args );
#else
#define ERROR( args... )
#endif

#if ERRORLEVEL > 2
#define WARN( args... ) __logError( 2, args );
#else
#define WARN( args... )
#endif

#if ERRORLEVEL > 3
#define TRACE( args... ) __logError( 3, args );
#else
#define TRACE( args... )
#endif

/**
 * Console print macros
 */
#if SERIAL_OUT

void PRINT( const char *fmt, ... );

#else

#define PRINT( args... )

#endif // SERIAL_OUT

#endif // DEBUG_H
