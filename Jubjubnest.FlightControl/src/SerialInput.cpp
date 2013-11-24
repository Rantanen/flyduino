
#include "SerialInput.h"
#include "FlightModel.h"
#include "debug.h"

_SerialInput SerialInput;

_SerialInput::_SerialInput()
{
}

void _SerialInput::update()
{
	while( Serial.available() > 0 )
	{
		char c = Serial.read();
		switch( c )
		{
			case '1':
				FlightModel.yawRatePID.Kp += 1.0;
				break;
			case '2':
				FlightModel.yawRatePID.Ki += 0.001;
				break;
			case '3':
				FlightModel.yawRatePID.Kd += 1;
				break;
			case 'q':
				FlightModel.yawRatePID.Kp -= 1.0;
				break;
			case 'w':
				FlightModel.yawRatePID.Ki -= 0.001;
				break;
			case 'e':
				FlightModel.yawRatePID.Kd -= 1;
				break;
		}

		Serial.print( "PID:\t" );
		Serial.print( FlightModel.yawRatePID.Kp );
		Serial.print( "\t" );
		Serial.print( FlightModel.yawRatePID.Ki, 4 );
		Serial.print( "\t" );
		Serial.println( FlightModel.yawRatePID.Kd );
	}
}

