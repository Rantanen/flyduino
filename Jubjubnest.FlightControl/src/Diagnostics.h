
#ifndef _DIAGNOSTICS_H_
#define _DIAGNOSTICS_H_

#include <Arduino.h>
#include "helper_3dmath.h"
#include "FlightModel.h"
#include "Engine.h"

#define MAX_ENGINES 8

#define DIAG_ORIENTATION 1
#define DIAG_HEADING 2
#define DIAG_ENGINE 3

#define DIAG_START(t) start(t)
#define DIAG_END() end()

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

