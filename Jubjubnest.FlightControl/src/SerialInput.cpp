
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
				FlightModel.pitchRatePID.Kp += 1;
				FlightModel.rollRatePID.Kp += 1;
				break;
			case '2':
				FlightModel.pitchRatePID.Ki += 0.001;
				FlightModel.rollRatePID.Ki += 0.001;
				break;
			case '3':
				FlightModel.pitchRatePID.Kd += 1;
				FlightModel.rollRatePID.Kd += 1;
				break;
			case 'q':
				FlightModel.pitchRatePID.Kp -= 1;
				FlightModel.rollRatePID.Kp -= 1;
				break;
			case 'w':
				FlightModel.pitchRatePID.Ki -= 0.001;
				FlightModel.rollRatePID.Ki -= 0.001;
				break;
			case 'e':
				FlightModel.pitchRatePID.Kd -= 1;
				FlightModel.rollRatePID.Kd -= 1;
				break;
		}

		Serial.print( "PID:\t" );
		Serial.print( FlightModel.pitchRatePID.Kp );
		Serial.print( "\t" );
		Serial.print( FlightModel.pitchRatePID.Ki, 4 );
		Serial.print( "\t" );
		Serial.println( FlightModel.pitchRatePID.Kd );
	}
}

