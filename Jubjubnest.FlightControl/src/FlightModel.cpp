

#include "FlightModel.h"
#include "debug.h"
#include "Quaternion.h"
#include "Diagnostics.h"

#define YAW_TARGET 1
#define PITCHROLL_TARGET 1
#define ENGINE_GAIN 0.1

FlightModel::FlightModel()
	: orientation(1,0,0,0), heading(1,0,0,0), power(3), engineCount(0), lastUpdate(0),
		yaw( 0 ), pitch( 0 ), roll( 0 ),
		yawOffset( 20, 0, 0 ), pitchOffset( 20, 0, 0 ), rollOffset( 20, 0, 0 ),
		yawSpeed( 10, 0, 0 ), pitchSpeed( 10, 0, 0 ), rollSpeed( 10, 0, 0 )
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

void FlightModel::updateOrientation( const Quat *orientation )
{
	this->orientation = *orientation;
}

void FlightModel::updateHeading( float yaw, float pitch, float roll, uint8_t power )
{
	heading.x = cos(yaw/2)*cos(pitch/2)*cos(roll/2) + sin(yaw/2)*sin(pitch/2)*sin(roll/2);
	heading.y = sin(yaw/2)*cos(pitch/2)*cos(roll/2) - cos(yaw/2)*sin(pitch/2)*sin(roll/2);
	heading.z = cos(yaw/2)*sin(pitch/2)*cos(roll/2) + sin(yaw/2)*cos(pitch/2)*sin(roll/2);
	heading.w = cos(yaw/2)*cos(pitch/2)*sin(roll/2) - sin(yaw/2)*sin(pitch/2)*cos(roll/2);

	this->power = power; 
}

void FlightModel::update()
{
	unsigned long currentTime = micros();

	Quat difference = heading.getProduct( orientation.getConjugate() );
	yaw = atan2(
				2*(difference.w*difference.z + difference.x*difference.y),
			1 - 2*(difference.y*difference.y + difference.z*difference.z) );
	pitch = asin(
			2*(difference.w*difference.y - difference.z*difference.x) );
	roll = atan2(
				2*(difference.w*difference.x + difference.y*difference.z),
			1 - 2*(difference.x*difference.x + difference.y*difference.y) );

	yawOffset.update( yaw, currentTime );
	pitchOffset.update( pitch, currentTime );
	rollOffset.update( roll, currentTime );

	/*
	yawOffset.resetError();
	pitchOffset.resetError();
	rollOffset.resetError();
	*/

	engines[0]->setPower( map(
			constrain( power - pitchOffset.getValue() - yawOffset.getValue() + rollOffset.getValue(), 0, 255 ),
			0, 255,
			0, 255 ) );
	engines[1]->setPower( map(
			constrain( power - pitchOffset.getValue() + yawOffset.getValue() - rollOffset.getValue(), 0, 255 ),
			0, 255,
			0, 255 ) );
	engines[2]->setPower( map(
			constrain( power + pitchOffset.getValue() - yawOffset.getValue() - rollOffset.getValue(), 0, 255 ),
			0, 255,
			0, 255 ) );
	engines[3]->setPower( map(
			constrain( power + pitchOffset.getValue() + yawOffset.getValue() + rollOffset.getValue(), 0, 255 ),
			0, 255,
			0, 255 ) );

	/*
	float yawDelta = (yaw - oldYaw) / delta;
	float idealYawDelta = yaw / YAW_TARGET;
	yawCorrection += (idealYawDelta - yawDelta ) * ENGINE_GAIN;

	TRACE( "Yaw: %i", (int)(yawCorrection * 1000) );

	for( int i = 0; i < engineCount; i++ ) 
	{
		VectorInt16 pos( engines[ i ]->x, engines[ i ]->y, engines[ i ]->z );
		pos.rotate( &difference );

		float engineOffset = pos.z;
		float engineOffsetDelta = ( engineOffset - oldEngineOffset[i] ) / delta;
		float idealEngineOffsetDelta = engineOffset / PITCHROLL_TARGET;

		engineOffsetCorrection[i] += ( idealEngineOffsetDelta - engineOffsetDelta ) * ENGINE_GAIN;
		oldEngineOffset[i] = engineOffset;

		TRACE( "Engine %i: %i", i, (int)(engineOffset * 1000) );
	}

	if( lastUpdate > 0 ) {
		for( int i = 0; i < engineCount; i++ )
		{
			bool isCW = (i % 2) == 0;

			uint8_t enginePower = constrain(
					power +
					yawCorrection * (isCW ? 1 : -1) +
					engineOffsetCorrection[i],
					0, 10 );

			engines[ i ]->setPower( enginePower );
		}
	} else {
		yawCorrection = 0;
		for( int i = 0; i < engineCount; i++ )
		{
			engineOffsetCorrection[i] = 0;
		}
	}

	lastUpdate = currentTime;
	*/
}

