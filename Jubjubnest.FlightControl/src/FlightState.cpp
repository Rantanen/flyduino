
#include "FlightState.h"
#include "Radio.h"
#include "FlightModel.h"
#include "common.h"
#include "debug.h"

_FlightState FlightState;

_FlightState::_FlightState()
	: currentState( eFlightStateOff ), currentStateController( 0 )
{
}

void _FlightState::update()
{
	updateChannelTimes();

	switch( currentState )
	{
		case eFlightStateOff:
			if( checkOffToArmed() )
				changeState( eFlightStateArmed, &FlightModel );
			break;

		case eFlightStateArmed:
			if( checkArmedToOff() );
				changeState( eFlightStateOff, 0 );
			break;
	}

	if( currentStateController != 0 )
		currentStateController->update();
}

void _FlightState::updateChannelTimes()
{
	uint32_t ms = millis();
	Channel* throttle = Radio.channels[2];

	// As long as the throttle isn't idling,
	// push the idle start to the future.
	if( throttle->raw > throttle->calibrationData.minValue + 50 )
		throttleIdleStart = ms + 1000;

	Channel* yaw = Radio.channels[3];

	// Same for yaw left/right
	if( yaw->raw > yaw->calibrationData.minValue + 50 )
		yawLeftStart = ms + 1000;
	if( yaw->raw < yaw->calibrationData.maxValue - 50 )
		yawRightStart = ms + 1000;
}

bool _FlightState::checkOffToArmed()
{
	uint32_t ms = millis();
	float throttle = Radio.channels[2]->value;

	// Make sure throttle is down and yaw is right (up)
	if( throttleIdleStart > ms || yawRightStart > ms )
	{
		digitalWrite( LED_PIN, LOW );
		return false;
	}

	if( ms > throttleIdleStart + STATE_CHANGE_TIME_OFF_TO_ARMED &&
			ms > yawRightStart + STATE_CHANGE_TIME_OFF_TO_ARMED )
	{
		return true;
	}

	blink(100);
	return false;
}

bool _FlightState::checkArmedToOff()
{
	uint32_t ms = millis();
	float throttle = Radio.channels[2]->value;

	// Make sure throttle is down
	if( throttleIdleStart > ms )
	{
		digitalWrite( LED_PIN, LOW );
		return false;
	}
	// Check whether engine has been idle long enough.
	if( ms > throttleIdleStart + STATE_CHANGE_TIME_OFF_ON_IDLE_THROTTLE )
	{
		return true;
	}

	// Blink here anyway for funs. Sure it'll light up the led a bit too much in some
	// cases, but that's for few microseconds and saves us some code size.
	blink(500);

	// Engine hasn't idled long enough. Check whether throttle/yaw combo tells
	// us to disarm.
	if( ms > throttleIdleStart + STATE_CHANGE_TIME_ARMED_TO_OFF &&
			ms > yawLeftStart + STATE_CHANGE_TIME_ARMED_TO_OFF )
	{
		return true;
	}

	blink(100);
	return false;
}

void _FlightState::changeState( FlightStates flightState, IFlightState* flightStateController )
{
	// Disable led in case it was left on after blinking.
	digitalWrite( LED_PIN, LOW );

	if( currentStateController != 0 )
		currentStateController->stop();

	currentStateController = flightStateController;
	currentState = flightState;

	if( currentStateController != 0 )
		currentStateController->start();
}
