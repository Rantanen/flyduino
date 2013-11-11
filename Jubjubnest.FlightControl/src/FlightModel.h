
#ifndef _FLIGHTMODEL_H_
#define _FLIGHTMODEL_H_

#include <arduino.h>
#include <helper_3dmath.h>
#include "Engine.h"
#include "PID.h"
#include "common.h"

/**
 * Flight model abstraction.
 *
 * Tracks all the sensory inputs for the vehicle.
 */
class FlightModel
{
	public:
		FlightModel();

		/**
		 * Adds a new engine record to the flight model
		 *
		 * @param engine Pointer to the engine
		 */
		void addEngine( Engine* engine );

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
		 * @param power Average engine power
		 * @param engineOn True if engines should be on
		 */
		void updateHeading( float yaw, float pitch, float roll, float power, bool engineOn );

		void setArmed( bool armed ) {
			Serial.print( "Armed: " );
			Serial.println( armed );
			digitalWrite( LED_PIN, !armed );
			this->armed = armed;
		}

		/**
		 * Adjusts the flight values
		 */
		void update();

		//! Orientation
		Quaternion orientation;

		//! Inteded power
		float power;

		//! Intended heading
		float controlYaw;
		float controlPitch;
		float controlRoll;

		//! True if the engine is on
		bool engineOn;

		//! True if the flight model is armed (able to fly)
		bool armed;

		//! Amount of engines present
		int engineCount;

		//! Engines present
		Engine* engines[ MAX_ENGINES ];

		unsigned long lastUpdate;
		unsigned long lastHeadingUpdate;

		float yaw;
		float pitch;
		float roll;

		PID yawOffset;
		PID pitchOffset;
		PID rollOffset;
};

#endif // _FLIGHTMODEL_H_
