
#include "Diagnostics.h"
#include "FlightModel.h"
#include "debug.h"

#define ALLOW_YIELD if( millis() > treshold ) \
{ \
	TRACE( "Reported %i steps", reports ); \
	return; \
}

Diagnostics::Diagnostics( uint16_t updateInterval )
	: interval( updateInterval ), nextUpdate( 0 ), engineStep( 0 ), step( 0 )
{
}

void Diagnostics::report( unsigned long ms )
{
	unsigned long treshold = millis() + 2;

	if( ms > nextUpdate ) {
		step = 1;
		nextUpdate = millis() + interval;
	}

	int reports = 0;
	while( step != 0 ) {

		switch( step )
		{
			case 1:
				DIAG( "ROT" );
				DIAG( FlightModel.orientation.x, 3 );
				DIAG( FlightModel.orientation.y, 3 );
				DIAG( FlightModel.orientation.z, 3 );
				DIAGLN( FlightModel.orientation.w, 3 );
				reports++;

				step++;
				ALLOW_YIELD;

			case 2:

				DIAG( "HEAD" );
				DIAG( FlightModel.heading.x, 3 );
				DIAG( FlightModel.heading.y, 3 );
				DIAG( FlightModel.heading.z, 3 );
				DIAGLN( FlightModel.heading.w, 3 );
				reports++;

				step++;
				ALLOW_YIELD;

			case 3:
				
				DIAG( "OFFSET" );
				DIAG( FlightModel.yaw, 3 );
				DIAG( FlightModel.pitch, 3 );
				DIAGLN( FlightModel.roll, 3 );
				reports++;

				step++;
				engineStep = 0;
				ALLOW_YIELD

			case 4:

				while( engineStep < FlightModel.engineCount )
				{
					DIAG( "ENG" );
					DIAG( FlightModel.engines[ engineStep ]->getPin() );
					DIAGLN( FlightModel.engines[ engineStep ]->getPower() );
					reports++;

					engineStep++;
					ALLOW_YIELD;
				}

				step++;
				ALLOW_YIELD;

			default:
				step = 0;
		}
	}
}

