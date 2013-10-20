

#include "FlightModel.h"
#include "debug.h"

void FlightModel::updateOrientation( const Quaternion *orientation )
{
	DIAG( "ROT" );
	DIAG( orientation->x, 6 );
	DIAG( orientation->y, 6 );
	DIAG( orientation->z, 6 );
	DIAGLN( orientation->w, 6 );

	this->orientation = *orientation;
}

void FlightModel::updateHeading( float yaw, float pitch, float roll )
{
	DIAG( "HEAD" );
	DIAG( yaw, 6 );
	DIAG( pitch, 6 );
	DIAGLN( roll, 6 );

	heading.x = roll;
	heading.y = pitch;
	heading.z = yaw;
}

