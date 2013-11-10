
#include "Engine.h"
#include <Arduino.h>
#include "common.h"

Engine::Engine( uint8_t pin )
	: _pin( pin )
{
}

bool Engine::setup()
{
	_servo.attach( _pin, MIN_ESC_PWM_WIDTH, MAX_ESC_PWM_WIDTH );
	_servo.write( MIN_ESC_PWM_WIDTH );
	return true;
}

void Engine::setPower( uint8_t power )
{
	_power = power;
	_servo.write( map(
				constrain( power, 0, 140 ),
				0, 255,
				MIN_ESC_PWM_WIDTH, ESC_PWM_LIMIT ) );
}


