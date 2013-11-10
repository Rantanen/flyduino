
#ifndef _DIAGNOSTICS_H_
#define _DIAGNOSTICS_H_

#include <Arduino.h>
#include "helper_3dmath.h"
#include "FlightModel.h"
#include "Engine.h"

class Diagnostics
{
	public:
		Diagnostics( uint16_t updateInterval );
		void setFlightModel( FlightModel* model );
		void report( unsigned long ms );

	private:
		uint16_t interval;
		unsigned long nextUpdate;

		uint8_t engineStep;
		uint8_t step;

		FlightModel* flightModel;

};

#endif

