
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
				FlightModel.pitchOffsetPID.Kp += 0.1;
				FlightModel.rollOffsetPID.Kp += 0.1;
				break;
			case '2':
				FlightModel.pitchOffsetPID.Ki += 0.001;
				FlightModel.rollOffsetPID.Ki += 0.001;
				break;
			case '3':
				FlightModel.pitchOffsetPID.Kd += 1;
				FlightModel.rollOffsetPID.Kd += 1;
				break;
			case 'q':
				FlightModel.pitchOffsetPID.Kp -= 0.1;
				FlightModel.rollOffsetPID.Kp -= 0.1;
				break;
			case 'w':
				FlightModel.pitchOffsetPID.Ki -= 0.001;
				FlightModel.rollOffsetPID.Ki -= 0.001;
				break;
			case 'e':
				FlightModel.pitchOffsetPID.Kd -= 1;
				FlightModel.rollOffsetPID.Kd -= 1;
				break;
		}

		Serial.print( "PID:\t" );
		Serial.print( FlightModel.pitchOffsetPID.Kp );
		Serial.print( "\t" );
		Serial.print( FlightModel.pitchOffsetPID.Ki, 4 );
		Serial.print( "\t" );
		Serial.println( FlightModel.pitchOffsetPID.Kd );
	}
}

