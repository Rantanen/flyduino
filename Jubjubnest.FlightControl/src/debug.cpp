
#include "arduino.h"
#include "debug.h"
#include <stdarg.h>

#if ERRORLEVEL > 0

char levels[][8] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG" };
char colors[][3] = { "31", "33", "32", "36", "22" };

void __logError( uint8_t level, const char *fmt, ... )
{
	char tmp[128];
	va_list args;
	va_start( args, fmt );
	vsnprintf( tmp, 128, fmt, args );
	va_end( args );
	Serial.print( "TRACE\t" );
	Serial.print( "\x1B[" );
	Serial.print( colors[level] );
	Serial.print( "m" );
	Serial.print( levels[level] );
	Serial.print( "\t" );
	Serial.print( tmp );
	Serial.println( "\x1B[m" );
}

#endif

#if SERIAL_OUT

void PRINT( const char *fmt, ... )
{
	char tmp[128];
	va_list args;
	va_start( args, fmt );
	vsnprintf( tmp, 128, fmt, args );
	va_end( args );
	Serial.print( "LOG\t" );
	Serial.println( tmp );
}

#endif
