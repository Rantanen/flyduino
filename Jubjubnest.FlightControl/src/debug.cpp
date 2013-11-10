
#include "arduino.h"
#include "debug.h"
#include <stdarg.h>

#if ERRORLEVEL > 0

char levels[][8] = { "FATAL", "ERROR", "WARN", "INFO", "DEBUG" };
char colors[][3] = { "31", "33", "32", "36", "22" };

void __logHeader( uint8_t level )
{
	Serial.print( "TRACE\t" );
	Serial.print( "\x1B[" );
	Serial.print( colors[level] );
	Serial.print( "m" );
	Serial.print( levels[level] );
	Serial.print( "\t" );
}

void __logErrorF( uint8_t level, const __FlashStringHelper* msg )
{
	__logHeader( level );
	Serial.print( msg );
}


void __logError( uint8_t level, const char *fmt, ... )
{
	char tmp[128];
	va_list args;
	va_start( args, fmt );
	vsnprintf( tmp, 128, fmt, args );
	va_end( args );
	Serial.print( tmp );
}

void __endError()
{
	Serial.println( "\x1B[m" );
}

#endif
