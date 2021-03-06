
#include "Radio.h"
#include "debug.h"
#include "Status.h"

_Radio Radio;

_Radio::_Radio()
	: channelCount( 0 )
{
}

_Radio::~_Radio()
{
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		delete channels[i];
	}
}

void _Radio::addChannel( int id )
{
	channels[ channelCount ] = new Channel( id );
	channelCount++;
	Status.channels = channelCount;
}

void _Radio::calibrate()
{
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->calibrate();
	}
}

void _Radio::recordCenterPositions()
{
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->storeCenter();
	}
}

void _Radio::saveCalibration()
{
	INFO( "Radio ranges by channel:" );
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->saveCalibration();
		INFO( "%i\t%u\t%u\t%u", i+1, channels[i]->calibrationData.minValue, channels[i]->calibrationData.center, channels[i]->calibrationData.maxValue );
	}
}

void _Radio::loadCalibration()
{
	INFO( "Radio ranges by channel:" );
	for( uint8_t i = 0; i < channelCount; i++ )
	{
		channels[i]->loadCalibration();
		INFO( "%i\t%u\t%u\t%u", i+1, channels[i]->calibrationData.minValue, channels[i]->calibrationData.center, channels[i]->calibrationData.maxValue );
	}
}

bool _Radio::update()
{
	int successful = 0;
	for( int i = 0; i < channelCount; i++ )
	{
		if( channels[ i ]->update() )
			successful++;
	}

	Serial.println( successful );
	return successful == channelCount;
}


