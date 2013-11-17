

#include "FlightModel.h"
#include "debug.h"
#include "Diagnostics.h"
#include "IMU.h"
#include "Radio.h"

_FlightModel FlightModel;

_FlightModel::_FlightModel()
	: orientation(1,0,0,0),
		yawRate(0), pitchRate(0), rollRate(0),
		power(0), controlYaw(0), controlPitch(0), controlRoll(0),
		engineCount(0),
#ifdef STABLE_MODE
		yawOffsetError( 0 ), pitchOffsetError( 0 ), rollOffsetError( 0 ),
		yawOffsetPID( PID_YAW_OFFSET_KP, PID_YAW_OFFSET_KI, PID_YAW_OFFSET_KD, PID_YAW_OFFSET_LIMIT ),
		pitchOffsetPID( PID_TILT_OFFSET_KP, PID_TILT_OFFSET_KI, PID_TILT_OFFSET_KD, PID_TILT_OFFSET_LIMIT ),
		rollOffsetPID( PID_TILT_OFFSET_KP, PID_TILT_OFFSET_KI, PID_TILT_OFFSET_KD, PID_TILT_OFFSET_LIMIT ),
#endif
		yawRateError( 0 ), pitchRateError( 0 ), rollRateError( 0 ),
		yawRatePID( PID_YAW_RATE_KP, PID_YAW_RATE_KI, PID_YAW_RATE_KD, PID_YAW_RATE_LIMIT ),
		pitchRatePID( PID_TILT_RATE_KP, PID_TILT_RATE_KI, PID_TILT_RATE_KD, PID_TILT_RATE_LIMIT ),
		rollRatePID( PID_TILT_RATE_KP, PID_TILT_RATE_KI, PID_TILT_RATE_KD, PID_TILT_RATE_LIMIT )
{
	for( int i = 0; i < MAX_ENGINES; i++ ) {
		engines[i] = 0;
	}
}

void _FlightModel::addEngine( Engine* engine )
{
	engines[ engineCount ] = engine;
	engineCount++;
}

void _FlightModel::readRadio()
{
	// Check that time has passed.
	unsigned long currentTime = micros();
	if( currentTime == lastHeadingUpdate ) return;

	// Update the control values
	controlYaw += YAW_CONTROL_GAIN * (Radio.channels[3]->value * (currentTime - lastHeadingUpdate) / 1000000.0f);
	controlPitch = -Radio.channels[1]->value;
	controlRoll = -Radio.channels[0]->value;

	if( Radio.channels[2]->value < 0.01 )
		power = 0;
	else
		power = Radio.channels[2]->value * ( MAX_CONTROL_POWER - MIN_CONTROL_POWER ) + MIN_CONTROL_POWER;

	lastHeadingUpdate = currentTime;
}

void _FlightModel::readIMU()
{
	this->orientation = IMU.orientation;

	this->yawRate = IMU.yawRate;
	this->pitchRate = IMU.pitchRate;
	this->rollRate = IMU.rollRate;
}

void setEnginePower( Engine* engine, float power )
{
	engine->setPower( constrain( power, 0, 1000 ) );
}

void _FlightModel::update()
{
	unsigned long currentTime = micros();

	// Update control values.
	readRadio();
	readIMU();

#ifdef STABLE_MODE

	// In stable mode we'll do quaternion matht o figure out orientaiton.

	// Since we suck at quaternions, calculate yaw separately.
	Quaternion yawRotation( cos( controlYaw / 2 ), 0, 0, sin( controlYaw / 2 ) );

	// Get the difference rotations
	Quaternion yawDifference = yawRotation.getProduct( orientation.getConjugate() );
	
	// Get the errors
	yawOffsetError = atan2(
				2*(yawDifference.w*yawDifference.z + yawDifference.x*yawDifference.y),
			1 - 2*(yawDifference.y*yawDifference.y + yawDifference.z*yawDifference.z) );

	rollOffsetError = controlRoll - atan2(
				2*(orientation.w*orientation.x + orientation.y*orientation.z),
			1 - 2*(orientation.x*orientation.x + orientation.y*orientation.y) );
	pitchOffsetError = controlPitch - asin(
			2*(orientation.w*orientation.y - orientation.z*orientation.x) );

	// In stable mode update offset pids
	yawOffsetPID.update( yawOffsetError, currentTime );
	pitchOffsetPID.update( pitchOffsetError, currentTime );
	rollOffsetPID.update( rollOffsetError, currentTime );

	// And include the offset PID outputs in the rate error.
	yawRateError = yawOffsetPID.getValue();
	pitchRateError = pitchOffsetPID.getValue();
	rollRateError = rollOffsetPID.getValue();

#else

	// In non-stable mode rate error is stick position directly.
	yawRateError = ACRO_YAW_GAIN * controlYaw;
	pitchRateError = ACRO_ROLL_GAIN * controlPitch;
	rollRateError =  ACRO_ROLL_GAIN * controlRoll;

#endif

	// Substract the current rate from the control values to finalize
	// the error calculation
	yawRateError -= yawRate;
	pitchRateError -= pitchRate;
	rollRateError -= rollRate;

	// Update the rate PIDs that drive the engines
	yawRatePID.update( yawRateError, currentTime );
	pitchRatePID.update( pitchRateError, currentTime );
	rollRatePID.update( rollRateError, currentTime );

	/*
	Serial.print( rollRatePID.Pe );
	Serial.print( "\t" );
	Serial.println( rollRatePID.De );
	*/

	// If power is 0, stop the engines completely.
	// Otherwise tweak them according to the PIDs.
	if( power == 0.0 )
	{
		stop();
	}
	else
	{
		setEnginePower( engines[0], power - pitchRatePID.getValue() - yawRatePID.getValue() + rollRatePID.getValue() );
		setEnginePower( engines[1], power - pitchRatePID.getValue() + yawRatePID.getValue() - rollRatePID.getValue() );
		setEnginePower( engines[2], power + pitchRatePID.getValue() - yawRatePID.getValue() - rollRatePID.getValue() );
		setEnginePower( engines[3], power + pitchRatePID.getValue() + yawRatePID.getValue() + rollRatePID.getValue() );
	}
}

void _FlightModel::start()
{
	lastHeadingUpdate = micros();
	controlYaw = atan2(
			2*(orientation.w*orientation.z + orientation.x*orientation.y),
		1 - 2*(orientation.y*orientation.y + orientation.z*orientation.z) );
}

void _FlightModel::stop()
{
	for( int i = 0; i < engineCount; i++ )
		engines[i]->setPower( 0 );
}

