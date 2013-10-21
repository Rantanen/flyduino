
#include "Radio.h"
#include "debug.h"

Radio::Radio( uint8_t sampleRate )
	: sampleRate( sampleRate ), interval( 1000 / sampleRate ), channelCount( 0 ), nextSample( 0 )
{
}

Radio::~Radio()
{
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		delete channels[i];
	}
}

void Radio::addChannel( int pin, float offset )
{
	channels[ channelCount ] = new Channel( pin, offset );
	channelCount++;
}

bool Radio::setup()
{
	uint8_t i;

	while( Serial.available() && Serial.read() );
	INFO( "Calibrating radio. Send any character to continue." );
	while( !Serial.available() ) {
		for( i = 0; i < channelCount; i++ )
		{
			channels[i]->calibrate();
		}
	}
	while( Serial.available() && Serial.read() );

	INFO( "Calibration done. Ranges by channel:" );
	for( i = 0; i < channelCount; i++ )
	{
		INFO( "%i\t%lu\t%lu\t(%i samples, %i timeouts)", i+1, channels[i]->minValue, channels[i]->maxValue, channels[i]->samples, channels[i]->timeouts );
	}

	return true;
}

bool Radio::shouldSample( unsigned long currentMillis )
{
	return currentMillis > nextSample;
}

void Radio::sample( unsigned long currentMillis )
{
	for( int i = 0; i < channelCount; i++ )
	{
		channels[ i ]->update();
	}

	nextSample = currentMillis + interval;
}


