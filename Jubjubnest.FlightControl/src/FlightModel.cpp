

#include "FlightModel.h"
#include "debug.h"
#include "Diagnostics.h"

FlightModel::FlightModel()
	: orientation(1,0,0,0), heading(1,0,0,0), power(0), engineCount(0), lastUpdate(0),
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

void FlightModel::updateHeading( float yaw, float pitch, float roll, float power )
{
	if( lastHeadingUpdate == 0 )
		lastHeadingUpdate = micros();

	unsigned long currentTime = micros();
	if( currentTime == lastHeadingUpdate ) return;

	controlYaw += YAW_CONTROL_GAIN * (yaw * (currentTime - lastHeadingUpdate) / 1000000.0f);
	lastHeadingUpdate = currentTime;

	heading.w = cos(roll/2)*cos(pitch/2)*cos(controlYaw/2) + sin(roll/2)*sin(pitch/2)*sin(controlYaw/2);
	heading.x = sin(roll/2)*cos(pitch/2)*cos(controlYaw/2) - cos(roll/2)*sin(pitch/2)*sin(controlYaw/2);
	heading.y = cos(roll/2)*sin(pitch/2)*cos(controlYaw/2) + sin(roll/2)*cos(pitch/2)*sin(controlYaw/2);
	heading.z = cos(roll/2)*cos(pitch/2)*sin(controlYaw/2) - sin(roll/2)*sin(pitch/2)*cos(controlYaw/2);

	this->power = power * MAX_CONTROL_POWER;
}

void setEnginePower( Engine* engine, uint8_t power, float offsetPower )
{
	if( power < 5 )
		engine->setPower( 0 );
	else
		engine->setPower( power + offsetPower );
}

void FlightModel::update()
{
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

	yawOffset.update( yaw, currentTime );
	pitchOffset.update( pitch, currentTime );
	rollOffset.update( roll, currentTime );

	/*
	yawOffset.resetError();
	pitchOffset.resetError();
	rollOffset.resetError();
	*/
	setEnginePower( engines[0], power, - pitchOffset.getValue() + yawOffset.getValue() + rollOffset.getValue() );
	setEnginePower( engines[1], power, - pitchOffset.getValue() - yawOffset.getValue() - rollOffset.getValue() );
	setEnginePower( engines[2], power, + pitchOffset.getValue() + yawOffset.getValue() - rollOffset.getValue() );
	setEnginePower( engines[3], power, + pitchOffset.getValue() - yawOffset.getValue() + rollOffset.getValue() );
}

