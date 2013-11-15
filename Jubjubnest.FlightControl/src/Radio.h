
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
		 * @param offset Possible offset for the channel
		 */
		void addChannel( int pin, float offset );

		/**
		 * Runs a single calibration step to find the limits of axis.
		 */
		void calibrate();

		/**
		 * Signals the calibration is done.
		 *
		 * Saves the calibration settings if they were valid.
		 */
		void calibrationDone();

		/**
		 * Loads previously calibrated radio values.
		 */
		void loadCalibration();

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
	private:

		//! Current channel count
		uint8_t channelCount;
};

extern _Radio Radio;

#endif // _RADIO_H_
