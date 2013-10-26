
#include "SerialInput.h"
#include "debug.h"

SerialInput::SerialInput()
	: read( 0 )
{
}

bool SerialInput::setup()
{
	servo.attach( 6, 1000, 4000 );
	return true;
}

void SerialInput::update()
{
	while( Serial.available() > 0 )
	{
		buffer[ read ] = Serial.read();
		if( buffer[ read ] == 0 )
		{
			processMsg();
			read = 0;
		}
		else
		{
			read++;
		}
	}
}

void SerialInput::processMsg()
{
	INFO( "Servo position: %i", buffer[0] );
	servo.write( buffer[0] );
}

