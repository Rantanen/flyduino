
#include "Encoder.h"

_Encoder Encoder;

void inline writeSegment( uint8_t length, uint8_t *buffer, uint8_t &checksum )
{
	Serial.write( length + 1 );
	checksum ^= ( length + 1 );

	for( int i = 0; i < length; i++ )
	{
		if( buffer[ i ] == 0 )
		{
			Serial.write( 0xff );
			Serial.write( 0xff );
			Serial.write( 0xff );
		}
		else
		{
			Serial.write( buffer[ i ] );
			checksum ^= buffer[ i ];
		}
	}
}

void _Encoder::WriteSerial( const uint8_t *ptr, uint16_t length )
{

	Serial.write( 0 );

	const uint8_t *end = ptr + length;
	uint8_t checksum = 0;

	uint8_t current_length = 0;
	uint8_t buffer[ 254 ];

	while( ptr < end )
	{
		if( *ptr == 0 )
		{
			writeSegment( current_length, buffer, checksum );
			current_length = 0;
		}
		else
		{
			buffer[ current_length++ ] = *ptr;

			if( current_length == 0xFE )
			{
				writeSegment( current_length, buffer, checksum );
				current_length = 0;
			}
		}

		ptr++;
	}

	Serial.write( checksum );
	Serial.write( 0 );
}

