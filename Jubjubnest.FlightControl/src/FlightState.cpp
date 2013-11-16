
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
	switch( currentState )
	{
		case eFlightStateOff:
			if( checkLeftStick( false, true, 4000 ) )
				changeState( eFlightStateArmed, &FlightModel );
			break;

		case eFlightStateArmed:
			if( checkLeftStick( false, false, 4000 ) || checkThrottle( false, 15000 ) )
				changeState( eFlightStateOff, 0 );
			break;
	}

	if( currentStateController != 0 )
		currentStateController->update();
}

bool _FlightState::checkLeftStick( bool throttleUp, bool yawRight, uint16_t time )
{
	float throttle = Radio.channels[2]->value;
	if( ( throttleUp && throttle < 0.95 ) || ( !throttleUp && throttle > 0.01 ) )
	{
		leftStickChangeTime = 0;
		return false;
	}

	float yaw = Radio.channels[3]->value;
	if( ( yawRight && yaw < 0.8 ) || ( !yawRight && yaw > 0.2 ) )
	{
		leftStickChangeTime = 0;
		return false;
	}

	blink( 100 );
	if( leftStickChangeTime == 0 )
	{
		leftStickChangeTime = millis() + time;
		return false;
	}

	return millis() > leftStickChangeTime;
}

bool _FlightState::checkThrottle( bool throttleUp, uint16_t time )
{
	float throttle = Radio.channels[2]->value;
	if( ( throttleUp && throttle < 0.95 ) || ( !throttleUp && throttle > 0.01 ) )
	{
		throttleChangeTime = 0;
		return false;
	}

	blink( 100 );
	if( throttleChangeTime == 0 )
	{
		throttleChangeTime = millis() + time;
		return false;
	}

	return millis() > throttleChangeTime;
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
