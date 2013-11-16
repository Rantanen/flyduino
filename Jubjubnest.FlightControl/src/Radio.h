
#ifndef _RADIO_H_
#define _RADIO_H_

#include "arduino.h"
#include "common.h"

#include "Channel.h"

/**
 * Radio control for sampling the radio input
 */
class _Radio
{
	public:

		/**
		 * Basic constructor
		 *
		 * @param sampleRate Intended sample rate for the radio control
		 */
		_Radio();

		/**
		 * Destructor
		 */
		~_Radio();

		/**
		 * Adds channel specification to the radio control
		 *
		 * @param pin Channel input pin
		 */
		void addChannel( int pin );

		/**
		 * Runs a single calibration step to find the limits of axis.
		 */
		void calibrate();

		/**
		 * Stores the center positions
		 */
		void recordCenterPositions();

		/**
		 * Loads previously calibrated radio values.
		 */
		void loadCalibration();

		/**
		 * Saves current calibrated radio values.
		 */
		void saveCalibration();

		/**
		 * Samples the radio inputs and updates the channel values
		 * 
		 * @param currentMillis Current time in milliseconds
		 *
		 * @returns True if all channels were successfully updated
		 */
		bool update();

		/**
		 * Channels controlled by this radio
		 */
		Channel* channels[ MAX_CHANNELS ];

		//! Current channel count
		uint8_t channelCount;
};

extern _Radio Radio;

#endif // _RADIO_H_
