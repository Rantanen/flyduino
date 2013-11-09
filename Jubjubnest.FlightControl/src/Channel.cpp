
#include "arduino.h"
#include "channel.h"
#include "debug.h"
#include "PinStatus.h"

#define ULONG_MAX 4294967295ul

Channel::Channel( uint8_t pin ):
	pin( pin ), offset( 0 ), minValue( 0 ), maxValue( 0 ), samples( 0 ), timeouts( 0 )
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
	minValue = ULONG_MAX;
	maxValue = 0;
}

void Channel::calibrate()
{
	unsigned int value = PinStatus::getValue( pin );
	if( value == 0 ) {
		WARN( "Timeout reading PWM during channel calibration" );
		timeouts++;
		return;
	}

	if( value > maxValue ) maxValue = value;
	if( value < minValue ) minValue = value;
	samples++;
}

void Channel::update()
{
	long duration = PinStatus::getValue( pin );
	if( duration == 0 && false )
	{
		ERROR( "Timeout reading channel PWM" );
		return;
	}
	value = (float)(duration - minValue) / (maxValue - minValue) + offset;
}
