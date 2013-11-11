
#include "Radio.h"
#include "debug.h"

Radio::Radio()
	: channelCount( 0 )
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
	pinMode( pin, INPUT );
	channelCount++;
}

void Radio::calibrate()
{
	while( Serial.available() && Serial.read() );
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->calibrate();
	}
}

void Radio::calibrationDone()
{
	INFO( "Radio ranges by channel:" );
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->calibrationDone();
		channels[i]->saveRange( i );
		INFO( "%i\t%lu\t%lu\t(%i samples, %i timeouts)", i+1, channels[i]->minValue, channels[i]->maxValue, channels[i]->samples, channels[i]->timeouts );
	}
}

void Radio::loadCalibration()
{
	INFO( "Radio ranges by channel:" );
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->loadRange( i );
		INFO( "%i\t%u\t%u", i+1, channels[i]->minValue, channels[i]->maxValue );
	}
}

bool Radio::update()
{
	int successful = 0;
	for( int i = 0; i < channelCount; i++ )
	{
		successful += channels[ i ]->update();
	}

	return successful == channelCount;
}


