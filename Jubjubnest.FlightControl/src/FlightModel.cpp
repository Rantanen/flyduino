

#include "FlightModel.h"
#include "debug.h"
#include "Diagnostics.h"

FlightModel::FlightModel()
	: orientation(1,0,0,0),
		power(0), controlYaw(0), controlPitch(0), controlRoll(0),
		engineOn(false), armed(true),
		engineCount(0), lastUpdate(0),
		yaw( 0 ), pitch( 0 ), roll( 0 ),
		yawOffset( PID_YAW_KP, PID_YAW_KI, PID_YAW_KD ),
		pitchOffset( PID_PITCH_KP, PID_PITCH_KI, PID_PITCH_KD ),
		rollOffset( PID_ROLL_KP, PID_ROLL_KI, PID_ROLL_KD )
{
	for( int i = 0; i < MAX_ENGINES; i++ ) {
		engines[i] = 0;
	}
}

void FlightModel::addEngine( Engine* engine )
{
	engines[ engineCount ] = engine;
	engineCount++;
}

void FlightModel::updateOrientation( const Quaternion *orientation )
{
	this->orientation = *orientation;
}

void FlightModel::updateHeading( float yaw, float pitch, float roll, float power, bool engineOn )
{
	// If this is first cycle, we have no basis for delta
	//  -> Set start point and return
	if( lastHeadingUpdate == 0 )
	{
		lastHeadingUpdate = micros();
		return;
	}

	// Check that time has passed.
	unsigned long currentTime = micros();
	if( currentTime == lastHeadingUpdate ) return;

	// If engine is off, reset yaw to the current heading.
	// Otherwise sum the current yaw input to the yaw.
	if( !engineOn )
	{
		controlYaw = atan2(
				2*(orientation.w*orientation.z + orientation.x*orientation.y),
			1 - 2*(orientation.y*orientation.y + orientation.z*orientation.z) );
	}
	else
	{
		controlYaw += YAW_CONTROL_GAIN * (yaw * (currentTime - lastHeadingUpdate) / 1000000.0f);
	}

	this->controlPitch = pitch;
	this->controlRoll = roll;

	this->power = power * ( MAX_CONTROL_POWER - MIN_CONTROL_POWER ) + MIN_CONTROL_POWER;
	this->engineOn = engineOn;

	// If we're not armed but engines were turned on, arm.
	// Also require that throttle is at zero.
	if( !armed && engineOn )
	{
		// If the engine is arming, keep blinking the led.
		digitalWrite( LED_PIN, ( millis() / 100 ) % 2 == 0 );

		if( power == 0 )
		{
			yawOffset.resetError();
			pitchOffset.resetError();
			rollOffset.resetError();
			setArmed( true );
		}
	}

	lastHeadingUpdate = currentTime;
}

void setEnginePower( Engine* engine, float power )
{
	Serial.print( power, 2 );
	//engine->setPower( constrain( power, 0, 1000 ) );
}

void FlightModel::update()
{
	if( !armed ) return;

	// If the engine is off, make sure engine power is 0 and return.
	if( !engineOn )
	{
		for( int i = 0; i < engineCount; i++ )
			engines[i]->setPower( 0 );

		// Once the engines have been forced to power 0,
		// set armed to false.
		setArmed( false );
		return;
	}

	unsigned long currentTime = micros();

	// Euler angles. :(
	// TODO: Figure out how to work with quaternions properly.
	Quaternion yawRotation( cos( controlYaw / 2 ), 0, 0, sin( controlYaw / 2 ) );
	Quaternion yawDifference = yawRotation.getProduct( orientation.getConjugate() );

	yaw = atan2(
				2*(yawDifference.w*yawDifference.z + yawDifference.x*yawDifference.y),
			1 - 2*(yawDifference.y*yawDifference.y + yawDifference.z*yawDifference.z) );
	roll = controlRoll - atan2(
				2*(orientation.w*orientation.x + orientation.y*orientation.z),
			1 - 2*(orientation.x*orientation.x + orientation.y*orientation.y) );
	pitch = controlPitch - asin(
			2*(orientation.w*orientation.y - orientation.z*orientation.x) );

	Serial.print( yaw, 6 );
	Serial.print( "\t" );
	Serial.print( pitch, 6 );
	Serial.print( "\t" );
	Serial.print( roll, 6 );

	yawOffset.update( yaw, currentTime );
	pitchOffset.update( pitch, currentTime );
	rollOffset.update( roll, currentTime );

	/*
	yawOffset.resetError();
	pitchOffset.resetError();
	rollOffset.resetError();
	*/

	Serial.print( "\tE1\t" );
	setEnginePower( engines[0], power - pitchOffset.getValue() - yawOffset.getValue() + rollOffset.getValue() );
	Serial.print( "\tE2\t" );
	setEnginePower( engines[1], power - pitchOffset.getValue() + yawOffset.getValue() - rollOffset.getValue() );
	Serial.print( "\tE3\t" );
	setEnginePower( engines[2], power + pitchOffset.getValue() - yawOffset.getValue() - rollOffset.getValue() );
	Serial.print( "\tE4\t" );
	setEnginePower( engines[3], power + pitchOffset.getValue() + yawOffset.getValue() + rollOffset.getValue() );
	Serial.println();
}

