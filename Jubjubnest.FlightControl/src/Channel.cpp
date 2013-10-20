
#include "arduino.h"
#include "channel.h"
#include "debug.h"

#define ULONG_MAX 4294967295ul
// Radio PWM signal should have 1500us wavelength
// Worst case scenario is 1500us to wait for the pin to go high and then
// 1500us for it to go low again.
#define TIMEOUT 1500*8

Channel::Channel( long pin ):
	pin( pin ), offset( 0 ), minValue( 0 ), maxValue( 0 ), samples( 0 ), timeouts( 0 )
{
	resetCalibration();
}

Channel::Channel( long pin, float offset ):
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
	unsigned long value = pulseIn( pin, HIGH, TIMEOUT );
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
	long duration = pulseIn( pin, HIGH, TIMEOUT );
	if( duration == 0 )
	{
		ERROR( "Timeout reading channel PWM" );
		return;
	}
	value = (float)(duration - minValue) / (maxValue - minValue) + offset;
}
