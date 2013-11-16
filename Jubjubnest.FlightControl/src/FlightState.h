
#ifndef _FLIGHTSTATE_H_
#define _FLIGHTSTATE_H_

#include <arduino.h>
#include "IFlightState.h"

enum FlightStates
{
	eFlightStateOff,
	eFlightStateArmed
};

class _FlightState
{
	public:
		_FlightState();
		FlightStates currentState;
		IFlightState* currentStateController;
		void update();

	private:
		bool checkLeftStick( bool throttleUp, bool yawRight, uint16_t time );
		bool checkThrottle( bool throttleUp, uint16_t time );
		void changeState( FlightStates flightState, IFlightState* flightStateController );

		uint32_t leftStickChangeTime;
		uint32_t throttleChangeTime;
};

extern _FlightState FlightState;

#endif
