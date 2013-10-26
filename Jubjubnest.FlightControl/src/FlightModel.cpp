

#include "FlightModel.h"
#include "debug.h"
#include "helper_3dmath.h"
#include "Diagnostics.h"

void FlightModel::addEngine( Engine* engine )
{
	engines[ engineCount ] = engine;
	engineCount++;
}

void FlightModel::updateOrientation( const Quaternion *orientation )
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
	Quaternion difference = heading.getProduct( orientation.getConjugate() );

	for( int i = 0; i < engineCount; i++ )
	{
		VectorInt16 pos( engines[ i ]->x, engines[ i ]->y, engines[ i ]->z );
		pos.rotate( &difference );
		engines[ i ]->setPower( power + pos.z );
	}
}

