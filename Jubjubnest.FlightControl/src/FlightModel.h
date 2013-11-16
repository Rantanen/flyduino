
#ifndef _FLIGHTMODEL_H_
#define _FLIGHTMODEL_H_

#include <arduino.h>
#include <helper_3dmath.h>
#include "common.h"
#include "Engine.h"
#include "PID.h"
#include "IFlightState.h"

/**
 * Flight model abstraction.
 *
 * Tracks all the sensory inputs for the vehicle.
 */
class _FlightModel : public IFlightState
{
	public:
		_FlightModel();

		/**
		 * Adds a new engine record to the flight model
		 *
		 * @param engine Pointer to the engine
		 */
		void addEngine( Engine* engine );

		/**
		 * Adjusts the flight values
		 */
		void update();

		void start();
		void stop();

		//! Orientation
		Quaternion orientation;

		float yawRate;
		float pitchRate;
		float rollRate;

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

#ifdef STABLE_MODE

		float yawOffsetError;
		float pitchOffsetError;
		float rollOffsetError;

		PID yawOffsetPID;
		PID pitchOffsetPID;
		PID rollOffsetPID;

#endif

		float yawRateError;
		float pitchRateError;
		float rollRateError;

		PID yawRatePID;
		PID pitchRatePID;
		PID rollRatePID;

	private:
		void readRadio();
		void readIMU();
};

extern _FlightModel FlightModel;

#endif // _FLIGHTMODEL_H_
