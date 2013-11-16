
#ifndef _FLIGHTSTATE_H_
#define _FLIGHTSTATE_H_

#include <arduino.h>
#include "common.h"
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
		void updateChannelTimes();

		bool checkOffToArmed();
		bool checkArmedToOff();

		void changeState( FlightStates flightState, IFlightState* flightStateController );

		uint32_t throttleIdleStart;
		uint32_t yawRightStart;
		uint32_t yawLeftStart;
};

extern _FlightState FlightState;

#endif
