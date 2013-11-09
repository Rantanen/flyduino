
#include "SerialInput.h"
#include "debug.h"

SerialInput::SerialInput()
	: read( 0 )
{
}

bool SerialInput::setup()
{
	return true;
}

void SerialInput::update()
{
	while( Serial.available() > 0 )
	{
		// If we've been ignoring, wait until 0 to reset status.
		if( read == -1 ) {
			if( Serial.read() == 0 ) { read = 0; }
			return;
		}

		buffer[ read ] = Serial.read();
		if( buffer[ read ] == 0 )
		{
			processMsg();
			read = 0;
		}
		else
		{
			read++;

			// If we're overflowing, start ignoring.
			if( read == 16 ) { read = -1; }
		}
	}
}

void SerialInput::processMsg()
{
	INFO( "Servo position: %i", buffer[0] );
	servo.write( buffer[0] );
}

