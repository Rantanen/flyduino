
#include "arduino.h"
#include "channel.h"
#include "debug.h"
#include "PinStatus.h"
#include <EEPROM.h>
#include "common.h"
#include "status.h"

Channel::Channel( uint8_t id )
	: id( id )
{
	resetCalibration();
}

void Channel::resetCalibration()
{
	calibrationData.minValue = 0;
	calibrationData.maxValue = 0;
}

void Channel::calibrate()
{
	uint16_t value = PinStatus::getValue( id );
	if( calibrationData.maxValue == 0 ||
			value > calibrationData.maxValue ) calibrationData.maxValue = value;
	if( calibrationData.minValue == 0 ||
			value < calibrationData.minValue ) calibrationData.minValue = value;
}

void Channel::storeCenter()
{
	uint16_t center = PinStatus::getValue( id );
	calibrationData.center = center;
	
	uint16_t maxOffset = max(
			calibrationData.center - calibrationData.minValue,
			calibrationData.maxValue - calibrationData.center );

	calibrationData.scale = 1.0f / maxOffset;
}

bool Channel::update()
{
	long duration = PinStatus::getValue( id );

	// If the input data is bad (signal lost) set output to zero.
	if( duration == 0 ) {
		WARN( "Bad values." );
		Status.channelData[ id ].raw = calibrationData.center;
		Status.channelData[ id ].value = 0;
		return false;
	}

	Status.channelData[ id ].raw = duration;
	duration = constrain( Status.channelData[ id ].raw, calibrationData.minValue, calibrationData.maxValue );
	Status.channelData[ id ].value = (duration - calibrationData.center) * calibrationData.scale;

	if( Status.channelData[ id ].value > 0 ) {
		if( Status.channelData[ id ].value < CHANNEL_DEADZONE )
			Status.channelData[ id ].value = 0;
		else
			Status.channelData[ id ].value -= CHANNEL_DEADZONE;
	}
	else
	{
		if( Status.channelData[ id ].value > -CHANNEL_DEADZONE )
			Status.channelData[ id ].value = 0;
		else
			Status.channelData[ id ].value += CHANNEL_DEADZONE;
	}

	return true;
}

void Channel::saveCalibration()
{
	uint16_t start = CHANNEL_EEPROM_START + sizeof( calibrationData ) * id;
	uint8_t* dataStart = reinterpret_cast<uint8_t*>( &calibrationData );
	for( uint8_t i = 0; i < sizeof( calibrationData ); i++ )
	{
		uint8_t data = *(dataStart + i);
		EEPROM.write( start + i, data );
	}
}

void Channel::loadCalibration()
{
	uint16_t start = CHANNEL_EEPROM_START + sizeof( calibrationData ) * id;
	uint8_t* dataStart = reinterpret_cast<uint8_t*>( &calibrationData );
	for( uint8_t i = 0; i < sizeof( calibrationData ); i++ )
	{
		uint8_t data = EEPROM.read( start + i );
		*(dataStart + i) = data;
	}
}
