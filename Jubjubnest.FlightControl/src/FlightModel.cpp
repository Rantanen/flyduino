

#include "FlightModel.h"
#include "debug.h"
#include "Diagnostics.h"

FlightModel::FlightModel()
	: orientation(1,0,0,0), heading(1,0,0,0), power(0), controlYaw(0), engineOn(false), armed(true),
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

	heading.w = cos(roll/2)*cos(pitch/2)*cos(controlYaw/2) + sin(roll/2)*sin(pitch/2)*sin(controlYaw/2);
	heading.x = sin(roll/2)*cos(pitch/2)*cos(controlYaw/2) - cos(roll/2)*sin(pitch/2)*sin(controlYaw/2);
	heading.y = cos(roll/2)*sin(pitch/2)*cos(controlYaw/2) + sin(roll/2)*cos(pitch/2)*sin(controlYaw/2);
	heading.z = cos(roll/2)*cos(pitch/2)*sin(controlYaw/2) - sin(roll/2)*sin(pitch/2)*cos(controlYaw/2);

	this->power = power * MAX_CONTROL_POWER;
	this->engineOn = engineOn;

	// If we're not armed but engines were turned on, arm.
	// Also require that throttle is at zero.
	if( !armed && engineOn && power == 0 )
	{
		setArmed( true );
	}

	lastHeadingUpdate = currentTime;
}

void setEnginePower( Engine* engine, float power )
{
	//engine->setPower( constrain( (int)power + offsetPower, 0, 255 ) );
}

void FlightModel::update()
{
	if( !armed ) return;

	// If the engine is off, make sure engine power is 0 and return.
	if( !engineOn )
	{
		for( int i = 0; i < engineCount; i++ )
			setEnginePower( engines[i], 0 );

		// Once the engines have been forced to power 0,
		// set armed to false.
		setArmed( false );
		return;
	}

	unsigned long currentTime = micros();

	Quaternion difference = heading.getProduct( orientation.getConjugate() );

	roll = atan2(
				2*(difference.w*difference.x + difference.y*difference.z),
			1 - 2*(difference.x*difference.x + difference.y*difference.y) );
	pitch = asin(
			2*(difference.w*difference.y - difference.z*difference.x) );
	yaw = atan2(
				2*(difference.w*difference.z + difference.x*difference.y),
			1 - 2*(difference.y*difference.y + difference.z*difference.z) );

	/*
	Serial.print( yaw, 6 );
	Serial.print( "\t" );
	Serial.print( pitch, 6 );
	Serial.print( "\t" );
	Serial.print( roll, 6 );
	Serial.print( "\t" );
	Serial.println( power );
	*/

	yawOffset.update( yaw, currentTime );
	pitchOffset.update( pitch, currentTime );
	rollOffset.update( roll, currentTime );

	/*
	yawOffset.resetError();
	pitchOffset.resetError();
	rollOffset.resetError();
	*/

	setEnginePower( engines[0], (float)power - pitchOffset.getValue() - yawOffset.getValue() + rollOffset.getValue() );
	setEnginePower( engines[1], (float)power - pitchOffset.getValue() + yawOffset.getValue() - rollOffset.getValue() );
	setEnginePower( engines[2], (float)power + pitchOffset.getValue() - yawOffset.getValue() - rollOffset.getValue() );
	setEnginePower( engines[3], (float)power + pitchOffset.getValue() + yawOffset.getValue() + rollOffset.getValue() );
	delay(1);
}

