
#include "Engine.h"
#include <Arduino.h>

#define MIN_WIDTH 1000
#define MAX_WIDTH 4000

#define MIN_VALUE 0
#define MAX_VALUE 100

Engine::Engine( uint8_t pin, int x, int y, int z )
	: x( x ), y( y ), z( z ), _pin( pin )
{
}

bool Engine::setup()
{
	_servo.attach( _pin, MIN_WIDTH, MAX_WIDTH );
	_servo.write( MIN_WIDTH );
	return true;
}

void Engine::setPower( uint8_t power )
{
	_power = power;
	_servo.write( map(
				constrain( power, MIN_VALUE, MAX_VALUE ),
				0, 255,
				MIN_WIDTH, MAX_WIDTH ) );
}


