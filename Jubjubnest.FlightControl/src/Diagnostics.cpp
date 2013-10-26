
#include "Diagnostics.h"
#include "debug.h"

#define ALLOW_YIELD if( millis() > treshold ) \
{ \
	TRACE( "Reported %i steps", reports ); \
	return; \
}

Diagnostics::Diagnostics( uint16_t updateInterval )
	: interval( updateInterval ), nextUpdate( 0 ), engineStep( 0 ), step( 0 ), flightModel( 0 )
{
}

void Diagnostics::setFlightModel( FlightModel* model )
{
	flightModel = model;
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
				DIAG( flightModel->orientation.x, 3 );
				DIAG( flightModel->orientation.y, 3 );
				DIAG( flightModel->orientation.z, 3 );
				DIAGLN( flightModel->orientation.w, 3 );
				reports++;

				digitalWrite( 13, flightModel->orientation.getMagnitude() > 1.05f );
				
				step++;
				ALLOW_YIELD;

			case 2:

				DIAG( "HEAD" );
				DIAG( flightModel->heading.x, 3 );
				DIAG( flightModel->heading.y, 3 );
				DIAG( flightModel->heading.z, 3 );
				DIAGLN( flightModel->orientation.w, 3 );
				reports++;

				step++;
				engineStep = 0;
				ALLOW_YIELD;

			case 3:

				while( engineStep < flightModel->engineCount )
				{
					DIAG( "ENG" );
					DIAG( flightModel->engines[ engineStep ]->getPin() );
					DIAGLN( flightModel->engines[ engineStep ]->getPower() );
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

