
#include "Engine.h"
#include "common.h"

Engine::Engine( uint8_t id, uint8_t pin )
	: _id( id ), _pin( pin )
{
}

bool Engine::setup()
{
	_servo.attach( _pin, MIN_ESC_PWM_WIDTH, MAX_ESC_PWM_WIDTH );
	_servo.write( MIN_ESC_PWM_WIDTH );
	return true;
}

void Engine::setPower( uint16_t power )
{
	Status.enginePower[ _id ] = power;
	power = map( constrain( power, 0, 750 ),
			0, 1000,
			MIN_ESC_PWM_WIDTH, ESC_PWM_LIMIT );

#ifdef LIVE
	_servo.write( power );
#endif
}


