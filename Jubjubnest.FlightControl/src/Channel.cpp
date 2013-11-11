
#include "arduino.h"
#include "channel.h"
#include "debug.h"
#include "PinStatus.h"
#include <EEPROM.h>
#include "common.h"

Channel::Channel( uint8_t pin ):
	pin( pin ), offset( 0 ),
	minValue( 0 ), maxValue( 0 ), samples( 0 ), timeouts( 0 )
{
	resetCalibration();
}

Channel::Channel( uint8_t pin, float offset ):
	pin( pin ), offset( offset ), minValue( 0 ), maxValue( 0 ), samples( 0 ), timeouts( 0 )
{
	resetCalibration();
}

void Channel::resetCalibration()
{
	minValue = UINT_MAX;
	maxValue = 0;
}

void Channel::calibrate()
{
	unsigned int value = PinStatus::getValue( pin );
	if( value > maxValue ) maxValue = value;
	if( value < minValue ) minValue = value;
	samples++;
}

void Channel::calibrationDone()
{
	minValue += 20;
	maxValue -= 20;
}

bool Channel::update()
{
	unsigned long duration = PinStatus::getValue( pin );

	// If the input data is bad (signal lost) set output to zero.
	if( duration == 0 ) {
		WARN( "Bad values." );
		raw = 0;
		value = 0;
		return false;
	}

	raw = duration;

	duration = constrain( raw, minValue, maxValue );
	value = (float)(duration - minValue) / (maxValue - minValue) + offset;

	if( value > 0 ) {
		if( value < CHANNEL_DEADZONE )
			value = 0;
		else
			value -= CHANNEL_DEADZONE;
	}
	else
	{
		if( value > -CHANNEL_DEADZONE )
			value = 0;
		else
			value += CHANNEL_DEADZONE;
	}

	return true;
}

void saveInt( unsigned int address, int value )
{
	uint8_t left = value >> 8;
	uint8_t right = value;
	EEPROM.write( address, left );
	EEPROM.write( address + 1, right );
}

int readInt( unsigned int address )
{
	uint8_t left = EEPROM.read( address );
	uint8_t right = EEPROM.read( address + 1 );
	return (left << 8) + right;
}

void Channel::saveRange( int slot )
{
	saveInt( CHANNEL_EEPROM_START + 4*slot, minValue );
	saveInt( CHANNEL_EEPROM_START + 4*slot + 2, maxValue );
}

void Channel::loadRange( int slot )
{
	minValue = readInt( CHANNEL_EEPROM_START + 4*slot );
	maxValue = readInt( CHANNEL_EEPROM_START + 4*slot + 2 );
}
