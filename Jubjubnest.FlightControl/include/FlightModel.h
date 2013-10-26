
#ifndef _FLIGHTMODEL_H_
#define _FLIGHTMODEL_H_

#include <arduino.h>
#include <helper_3dmath.h>

/**
 * Flight model abstraction.
 *
 * Tracks all the sensory inputs for the vehicle.
 */
class FlightModel
{
	public:
		/**
		 * Updates the orientation with new information.
		 *
		 * @param orientation New orientation.
		 */
		void updateOrientation( const Quaternion *orientation );

		/**
		 * Updates the heading with new information.
		 *
		 * 0,0,0 means the craft should be upright.
		 *
		 * @param yaw Yaw Euler angle
		 * @param pitch Pitch Euler angle
		 * @param roll Roll Euler angle
		 */
		void updateHeading( float yaw, float pitch, float roll );

		//! Orientation
		Quaternion orientation;

		//! Intended vehicle heading
		VectorFloat heading;
};

#endif // _FLIGHTMODEL_H_
